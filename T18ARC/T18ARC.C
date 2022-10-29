/* Naumenko-Zhivoy Artem, 09-2, 21.10.2022, AN2 */
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <glut.h>

#include "arc.h"
#include "gfx.h"

double Zoom = 4;

void Display( void )
{
  static long t;
  static long start_game = -1, start_ball = 0;
  
  t = clock();

  if (start_game == -1)
    start_game = start_ball = t, ArcDrawGame();

  if (t - start_ball > 0.005 * CLOCKS_PER_SEC)
  {
    start_ball = t;
    ArcTick();
  }
  
  ArcDrawGame();
}

void Init( void );
void Keyboard( byte key, int x, int y )
{
  switch (key) 
  {
    case 'q':
    case 'Q':
      exit(0);
      break;
    case ' ':
      Init();
      break;
  }
}

void Init( void )
{
  if (ArcGetGame() == GAME_START || ArcGetGame() == GAME_LOSE || ArcGetGame() == GAME_WIN) 
  {
    ArcInitBricks();
    ArcResetScore();
    ArcSetBall(40, 80, 4, 4, rand() > RAND_MAX / 2 ? 1 : -1, -1);
    ArcSetBallColor(rand() * rand());

    printf("%d\n", GFX_getHexColor(255, 128, 64));
    ArcSetPaddle(62, H - 24, 40, 2);
    ArcSetPaddleColor(rand() * rand());
  }
}

void main( void )
{
  int scaleConstant = 8 / Zoom;

  srand(time((unsigned)0));
  GFX_LoadFont();

  glutInitDisplayMode(GLUT_RGB);
  glutInitWindowSize(W * Zoom, H * Zoom);
  glutInitWindowPosition(90, 40);
  glutCreateWindow("AN2 Graphics");
  /* glutFullScreen(); */
  glutDisplayFunc(Display);
  glutKeyboardFunc(Keyboard);

  glClearColor(0.30, 0.47, 0.8, 1);

  Init();

  glutMainLoop();
}

