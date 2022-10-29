/* Naumenko-Zhivoy Artem, 09-2, 21.10.2022, AN2 */
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <glut.h>

#include "gfx.h"

static dword Frame[W * H];

FILE *fontFile;
static byte Font[16 * 256];

double Zoom;

void GFX_LoadFont( void )
{
  printf("Opening file...\n");
  fontFile = fopen("Font1.fnt", "rb");

  if (fontFile == NULL)
  {
    printf("Failed to load file\n");
    getchar();
    return;
  }
  
  fread(Font, FONT_HEIGHT, 256, fontFile);
  fclose(fontFile);
  printf("File opened...\n");
}


void GFX_PutPixel( int x, int y, dword Color)
{
  if (x >= 0 && x < W && y >= 0 && y < H)
    Frame[y * W + x] = Color;
}

void GFX_DrawRect( int x, int y, int w, int h, dword Color )
{
  GFX_DrawStraightLine(x, y, x + w, y, Color);
  GFX_DrawStraightLine(x, y, x, y + h, Color);
  GFX_DrawStraightLine(x, y + h, x + w, y + h, Color);
  GFX_DrawStraightLine(x + w, y, x + w, y + h, Color);
}

void GFX_FillRect( int x, int y, int w, int h, dword Color )
{
  int i, j;

  for (j = 0; j < h; j++)
    for (i = 0; i < w; i++)
      GFX_PutPixel(x + i, y + j, Color);    
}

void GFX_DrawStraightLine( int x1, int y1, int x2, int y2, dword Color )
{
  int i;

  if (x1 == x2)
    for (i = y1; i <= y2; i++)
      GFX_PutPixel(x1, i, Color);
  if (y1 == y2)
    for (i = x1; i <= x2; i++)
      GFX_PutPixel(i, y1, Color);
}

void GFX_DrawChar( int x, int y, char c, dword Color, int shadow )
{
  int i, j;
  
  for (j = 0; j < FONT_HEIGHT; j++)
    for (i = 0; i < FONT_WIDTH; i++)
      if ((Font[(byte)c * FONT_HEIGHT + j] >> (FONT_WIDTH - 1 - i)) & 1)
      {            
        GFX_PutPixel(x + i, y + j, Color);
        if (shadow)
          GFX_PutPixel(x + i + 2, y + j + 2, (dword)( 0.2 * Color));
      }      
}

void GFX_DrawString( int x, int y, int verticalGap, char *str, dword color, int shadow )
{
  int i, cur, length = strlen(str), lines;
  
  for (i = 0, cur = 0, lines = 0; i < length; i++)           
  {
    if (str[i] != '\n')
      GFX_DrawChar(x + FONT_WIDTH * cur++, y + verticalGap * lines, str[i], color, shadow);
    else
      lines++, cur = 0;
  }
}

/* get last part of the str from delim */
static char* strsep(char** stringp, const char* delim)
{
  char* token_start = *stringp;
  
  if (*stringp == NULL)
      return NULL;
  
  *stringp = strpbrk(token_start, delim); /* index of delim char in token_start */
  if (*stringp) /* if this char exists (!= 0) */
  {
    **stringp = '\0';
    (*stringp)++;
  }
  return token_start;
}

/* copy a string */
static char* copyString( char* str )
{
  static char* res;

  res = malloc(strlen(str) + 1);
  if (res == NULL)
    return NULL;

  strcpy(res, str);

  return res;
}

static int countChar( char *str, char toCount )
{
  int i, res;

  for (i = 0, res = 0; str[i] != 0; i++)
    if (str[i] == toCount)
      res++;

  return res;
}

/* Drawing a centered string with multiple lines in it */
void GFX_DrawCenteredString( char* str, int verticalGap, dword color, int shadow )
{
  char *token, *string; 
  int cur = 0, length, x, y, count, blockHeight;

  string = copyString(str); /* copying to prevent changes in original string */
  count = countChar(str, '\n');

  blockHeight = (FONT_HEIGHT + verticalGap) * count;
  y = H / 2 - blockHeight; /* beginning from h / 2 - blockHeight*/
  
  while((token = strsep(&string, "\n")))
  {
    length = strlen(token);
    x = W / 2 - FONT_WIDTH * length / 2; /* every string is centered by it's x */
    y += FONT_HEIGHT + verticalGap; /* y is changed, because we don't know Y individually for each line */
    cur++;

    GFX_DrawString(x, y, verticalGap, token, color, shadow);
  }
  free(string);
}

dword GFX_getHexColor( byte R, byte G, byte B )
{
  return R << 16 | G << 8 | B;
}

ColorRGB GFX_getByteColor( dword hexColor )
{
  ColorRGB res;

  res.R = hexColor >> 16 && 0xFF;
  res.G = hexColor >> 8 && 0xFF;
  res.B = hexColor && 0xFF;

  return res;
}

void GFX_clearFrame( void )
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  glRasterPos2d(-1, 1);
  glPixelZoom(Zoom, -Zoom);
  
  memset(Frame, 0, sizeof(Frame));
}

void GFX_drawFrame( void )
{
  glDrawPixels(W, H, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Frame);

  glFinish();
  glutSwapBuffers();
  glutPostRedisplay();
}
