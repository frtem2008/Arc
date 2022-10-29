/* Naumenko-Zhivoy Artem, 09-2, 21.10.2022, AN2 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "arc.h"
#include "gfx.h"

static int ballSet = 0, paddleSet = 0;

static rectangle ballPosSize;
static int2 ballSpeed;
static dword ballColor;

static rectangle paddlePosSize;
static dword paddleColor;

static int score = 0;
static int game = GAME_START;

static char Bricks[BRICK_FIELD_W][BRICK_FIELD_H]; /* not byte because of negative durability when explosions */
static rectangle BrickPositions[BRICK_FIELD_W][BRICK_FIELD_H];

static int AutoGameFlag = 0;
static long CAPS_LOCK_LAST_PRESS = -1;

static char* getWinGameStr()
{
  static char str[100];
  
  memset(str, 0, 100 * sizeof(char));
  sprintf(str, "You win, score is: %d\3\n(Press spacebar to restart)\nABOBA\0", score);

  return str;
}

static char* getLoseGameStr()
{
  static char str[100];

  memset(str, 0, 100 * sizeof(char));
  sprintf(str, "You lose, score is: %d\3\n(Press spacebar to restart)\nABOBA\0", score);

  return str;
}

static int touchX( rectangle toTouch )
{
  if (ballPosSize.x >= toTouch.x && ballPosSize.x + ballPosSize.w <= toTouch.x + toTouch.w)
    return 1;
  return 0;
}

static int touchY( rectangle toTouch )
{
  if (ballPosSize.y + ballPosSize.h >= toTouch.y && ballPosSize.y <= toTouch.y + toTouch.h)
    return 1;
  return 0;
}


static int touchPaddle( void )
{
  if (ballSpeed.y <= 0) 
    return 0;
  
  if (touchX(paddlePosSize) && touchY(paddlePosSize))
      return 1;
  return 0;
}

static char* getScoreStr()
{
  static char res[15];
  
  memset(res, 0, 15 * sizeof(char));
  sprintf(res, "Score is %d\3", score);

  return res;
}

static void drawScore( void )
{
  GFX_DrawString(2, H - 17, 1, getScoreStr(), SCORE_COLOR, 1);
}

static void drawBall( void )
{
  GFX_FillRect(ballPosSize.x, ballPosSize.y, ballPosSize.w, ballPosSize.h, ballColor); 
}

void HandleBricks( void );

static void updateballPosition( void )
{
  HandleBricks();

  if (ballPosSize.x - 1 < 0 || ballPosSize.x + ballPosSize.w + 1 >= W)
    ballSpeed.x = - ballSpeed.x;
  if (ballPosSize.y - 1 < 0 || ballPosSize.y + ballPosSize.h + 2 >= H || touchPaddle())
    ballSpeed.y = - ballSpeed.y;

  ballPosSize.x += ballSpeed.x;
  ballPosSize.y += ballSpeed.y;

  if (ballPosSize.y + ballPosSize.h + 2 >= H)
    game = GAME_LOSE;

}

void ArcSetBallColor( dword color )
{
  ballColor = color;
}

void ArcSetBall(int x, int y, int w, int h, int xSpeed, int ySpeed)
{
  game = GAME_IN_PROCESS;

  ballPosSize.x = x;
  ballPosSize.y = y;
  ballPosSize.w = w;
  ballPosSize.h = h;
  ballSpeed.x = xSpeed;
  ballSpeed.y = ySpeed;
}

static void drawPaddle( void )
{
  GFX_FillRect(paddlePosSize.x, paddlePosSize.y, paddlePosSize.w, paddlePosSize.h, paddleColor);
  GFX_DrawString(W - 15 * 8, H - 17, 1, AutoGameFlag ? "Auto mode:  on" : "Auto mode: off", AUTO_MODE_COLOR, 1);
}

static void drawBottom( void )
{
  GFX_FillRect(0, H - 1, W, 2, 0xFF0000);
}

static void updatePaddlePosition( void )
{
  static int t;

  t = clock();                  
  if ((GetAsyncKeyState(VK_CAPITAL) & 0x8000) && t - CAPS_LOCK_LAST_PRESS > 0.2 * CLOCKS_PER_SEC)
  {
    CAPS_LOCK_LAST_PRESS = t;
    AutoGameFlag = !AutoGameFlag;
  }

  if (paddlePosSize.x - 1 > 0)
    if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) && !AutoGameFlag ||
      AutoGameFlag && ballPosSize.x + ballPosSize.w / 2 < paddlePosSize.x + paddlePosSize.w / 2)
        paddlePosSize.x -= PADDLE_SPEED;
  if (paddlePosSize.x + paddlePosSize.w + 1 <= W)
      if ((GetAsyncKeyState(VK_RCONTROL) & 0x8000) && !AutoGameFlag || 
        AutoGameFlag && ballPosSize.x + ballPosSize.w / 2 > paddlePosSize.x + paddlePosSize.w / 2)
       paddlePosSize.x += PADDLE_SPEED;
}

void ArcSetPaddle( int x, int y, int width, int height )
{
  paddlePosSize.x = x;
  paddlePosSize.y = y;
  paddlePosSize.w = width;
  paddlePosSize.h = height;
}

void ArcSetPaddleColor( dword color )
{
  paddleColor = color;
}

static void ExplodeBrick( int x, int y )
{
  int i, j;

  for (j = -BRICK_EXPLODE_RANGE; j <= BRICK_EXPLODE_RANGE; j++)
  {
    for (i = -BRICK_EXPLODE_RANGE; i <= BRICK_EXPLODE_RANGE; i++)
      if (rand() / (double)RAND_MAX * 100 < BRICK_EXPLODE_CHANCE)
        if (x + i > -1 && x + i < BRICK_FIELD_W && y + j > -1 && y + j < BRICK_FIELD_H)
        {
          Bricks[x + i][y + j]--, score++;
          if (Bricks[x + i][y + j]-- == 3 && rand() / (double)RAND_MAX * 100 < BRICK_NEAR_EXPLODE_CHANCE) /* Explode nearby bricks*/
            ExplodeBrick(x + i, y + j);
        }
  }
  Bricks[x][y] = 0;
}

static int Hit( int x, int y )
{
  if (x < BRICK_BEGIN_X || y < BRICK_BEGIN_Y)
    return 0;
  x -= BRICK_BEGIN_X;
  y -= BRICK_BEGIN_Y;

  if ((x /= BRICK_W) >= BRICK_FIELD_W || (y /= BRICK_H) >= BRICK_FIELD_H)
    return 0;
  if (Bricks[x][y] <= 0)
    return 0;
  
  if (Bricks[x][y] == 4)
    ExplodeBrick(x, y);

  Bricks[x][y]--;
  
#ifdef AUDIOMODE
  printf("\a"); /* for sound when hit */
#endif  
  score++;

  return 1;
}

void HandleBricks( void )
{
  if (Hit(ballPosSize.x, ballPosSize.y + ballSpeed.y))
    ballSpeed.y = -ballSpeed.y;
  if (Hit(ballPosSize.x + ballSpeed.x, ballPosSize.y))
    ballSpeed.x = -ballSpeed.x;
  if (Hit(ballPosSize.x + ballSpeed.x, ballPosSize.y + ballSpeed.y))
    ballSpeed.x = -ballSpeed.x, ballSpeed.y = -ballSpeed.y;
}

void ArcInitBricks( void )
{
  int i, j;
  rectangle cur;

  for (j = 0; j < BRICK_FIELD_H; j++)
    for (i = 0; i < BRICK_FIELD_W; i++)
    {
      Bricks[i][j] = rand() % 3 + 1;
      if (Bricks[i][j] == 3 && rand() / (double) RAND_MAX * 100 < EXPLOSIVE_BRICK_CHANCE)
        Bricks[i][j] = 4;

      cur.x = BRICK_BEGIN_X + i * BRICK_W + 1;
      cur.y = BRICK_BEGIN_Y + j * BRICK_H + 1;
      cur.w = BRICK_W;
      cur.h = BRICK_H;

      BrickPositions[i][j] = cur;
    }
}

static dword GetBrickColor( int i, int j )
{
  int br = Bricks[i][j];
  
  switch (br)
  {
    case 1:
      return (dword) BRICK_COLOR_ONE;
    case 2:
      return (dword) BRICK_COLOR_TWO;
    case 3:
      return (dword) BRICK_COLOR_THREE;
    case 4:
      return (dword) BRICK_COLOR_EXPLOSIVE;
    case 0:
      return (dword) BRICK_COLOR_DEAD;
  }
  return (dword) BRICK_COLOR_DEAD;
}

static void drawBricks( void )
{
  int i, j;

  for (j = 0; j < BRICK_FIELD_H; j++)
    for (i = 0; i < BRICK_FIELD_W; i++)
      GFX_DrawRect(BrickPositions[i][j].x, BrickPositions[i][j].y, BRICK_W - 2, BRICK_H - 2, GetBrickColor(i, j));
}

static void CheckBricks( void )
{
  int i, j;

  for (j = 0; j < BRICK_FIELD_H; j++)
    for (i = 0; i < BRICK_FIELD_W; i++)
      if (Bricks[i][j] > 0) 
        return;
  game = GAME_WIN;
}

void ArcResetScore( void )
{
  score = 0;
}

int ArcGetGame( void ) 
{
  return game;
}

void ArcDrawGame( void )
{
  GFX_clearFrame();
  
  if (game == GAME_IN_PROCESS)
  {
    drawBottom();
    drawBricks();
    drawBall();
    drawPaddle();
    drawScore();
  }

  if (game == GAME_LOSE)
  { 
    GFX_DrawCenteredString(getLoseGameStr(), 2, 0xFFFFFF, 1);
    system("powershell -c (New-Object Media.SoundPlayer 'Shit.wav').PlaySync();");
  }
  if (game == GAME_WIN) GFX_DrawCenteredString(getWinGameStr(), 2, 0xFFFFFF, 1);

  GFX_drawFrame();
}


void ArcTick(void)
{
  if (game == GAME_IN_PROCESS)
  {
    updateballPosition();
    updatePaddlePosition();
    CheckBricks();
  }
}

