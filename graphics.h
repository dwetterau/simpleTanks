#ifndef H_GRAPHICS
#define H_GRAPHICS
#include <SDL/SDL.h>
#define HEIGHT (600)
#define WIDTH (800)
/**
 *
 * Written by David Wetterau
 *
 */
void DrawLine(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, float x0, float y0, float x1 ,float y1);
void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y);
char clip(float* x0,float* y0,float* x1,float* y1);
#endif
