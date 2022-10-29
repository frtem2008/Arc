/* Naumenko-Zhivoy Artem, 09-2, 21.10.2022, AN2 */
#define BRICK_BEGIN_X 10
#define BRICK_BEGIN_Y 10
                        
#define BRICK_W 14
#define BRICK_H 10

#define BRICK_FIELD_W 16
#define BRICK_FIELD_H 7

#define EXPLOSIVE_BRICK_CHANCE 10    /* in persent */
#define BRICK_EXPLODE_CHANCE 50      /* in persent */
#define BRICK_NEAR_EXPLODE_CHANCE 80 /* in persent */
#define BRICK_EXPLODE_RANGE 2

#define BRICK_COLOR_DEAD 0x000000
#define BRICK_COLOR_EXPLOSIVE 0xFF5252
#define BRICK_COLOR_ONE 0x82B1FF
#define BRICK_COLOR_TWO 0xCCCC44
#define BRICK_COLOR_NEGATIVE 0xFFFFFF
#define BRICK_COLOR_THREE 0x406739 /* Legendary Timur's random number for green holy shit 406739 */

#define PADDLE_SPEED 1

#define SCORE_COLOR 16744512
#define AUTO_MODE_COLOR 0xADFCA     

#define GAME_LOSE -1
#define GAME_WIN 100
#define GAME_START 1
#define GAME_IN_PROCESS 10

/* #define AUDIOMODE */

typedef unsigned char byte;
typedef unsigned dword;

void ArcSetBall(int x, int y, int w, int h, int xSpeed, int ySpeed);
void ArcSetBallColor( dword color );

void ArcSetPaddle( int x, int y, int width, int height );
void ArcSetPaddleColor( dword color );

void ArcDrawGame( void );

void ArcInitBricks( void );
void ArcResetScore( void );

void ArcTick( void );
int ArcGetGame( void );
