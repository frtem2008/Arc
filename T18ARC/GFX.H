/* Naumenko-Zhivoy Artem, 09-2, 21.10.2022, AN2 */
#include <stdio.h>

#ifndef W
#define W 284
#endif

#ifndef H
#ifdef laptop
#define H 168 
#else
#define H 204
#endif // laptop
#endif

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

typedef unsigned char byte;
typedef unsigned dword;

extern byte Font[FONT_HEIGHT * 256];
extern FILE *fontFile;

extern double Zoom;

typedef struct {
  byte R, G, B;
} ColorRGB;

typedef struct {
  int x, y;
} int2; 

typedef struct {
  int x, y, w, h;
} rectangle;

void GFX_LoadFont( void );
void GFX_DrawChar( int x, int y, char c, dword Color, int shadow );
void GFX_DrawString( int x, int y, int gap, char *str, dword color, int shadow );
void GFX_DrawCenteredString(char* str, int gap, dword color, int shadow);

void GFX_PutPixel( int x, int y, dword Color);
void GFX_DrawRect( int x, int y, int w, int h, dword Color);
void GFX_FillRect( int x, int y, int w, int h, dword Color);
void GFX_DrawStraightLine( int x1, int y1, int x2, int y2, dword Color );

void GFX_clearFrame( void );
void GFX_drawFrame( void );

dword GFX_getHexColor( byte R, byte G, byte B );
ColorRGB GFX_getByteColor( dword hexColor );
