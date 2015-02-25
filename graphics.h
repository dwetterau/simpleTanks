#ifndef H_GRAPHICS
#define H_GRAPHICS
#include <SDL/SDL.h>
#define HEIGHT (768)
#define WIDTH (1024)
/**
 *
 * Written by David Wetterau
 *
 */
void DrawLine(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, float x0, float y0, float x1 ,float y1);
void DrawString(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, char* str, int x, int y);
void DrawLetter(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, char c, int x, int y);
void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y);
char clip(float* x0,float* y0,float* x1,float* y1);
#endif
