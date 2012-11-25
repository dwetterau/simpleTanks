#include "graphics.h"
#include <SDL/SDL.h>
#include <math.h>
#include "charMap.h"

/**
 *
 * Written by David Wetterau
 *
 */

const float NUMBER_PIXELS_X = WIDTH-.5;
const float NUMBER_PIXELS_Y = HEIGHT-.5;

void DrawLine(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, float x0, float y0, float x1, float y1){
    y0 = NUMBER_PIXELS_Y - y0;
    y1 = NUMBER_PIXELS_Y - y1;
    char ret = 1;
    if(!(x0>=0&&x1>=0&&x0<NUMBER_PIXELS_X&&x1<NUMBER_PIXELS_X&&y0>=0&&y1>=0&&y0<NUMBER_PIXELS_Y&&y1<NUMBER_PIXELS_Y)){
        ret = clip(&x0,&y0,&x1,&y1);
        //printf("clipped\n");
    }
    if(!ret)
        return; //both points are not in the screen
    //printf("%f %f\n",y0,y1);
    if(round(x0) == round(x1)){ //Vertical Line Case:
        float y;
        if(y0<y1){
            for(y = y0; y <= y1 ; y++){
                DrawPixel(screen,R,G,B,round(x0),round(y));
            }
        }
        else{
            for(y = y0; y > y1 ; y--){
                DrawPixel(screen,R,G,B,round(x0),round(y));
            }
        }
    }
    else{
        float slope = (y1-y0)/(x1-x0);
        if(fabs(slope) <= 1){ //increment x
            int inc = x0<x1 ? 1 : -1;
            float x;
            for(x = x0; fabs(x1-x) > .5; x += inc){
                float t = (x-x0)/(x1-x0);
                float y = y0 + t*(y1-y0);
                DrawPixel(screen,R,G,B,round(x),round(y));
            }
        }
        else{ //increment y
            int inc = y0<y1 ? 1 : -1;
            float y;
            for(y = y0; fabs(y1-y) > .5; y += inc){
                float t = (y-y0)/(y1-y0);
                float x = x0 + t*(x1-x0);
                DrawPixel(screen,R,G,B,round(x),round(y));
            }
        }
    }
}
char clip(float* x0, float* y0, float* x1, float*y1){
    char flag1,flag2;
    flag1 = (*x0>=0&&*x0<NUMBER_PIXELS_X&&*y0>=0&&*y0<NUMBER_PIXELS_Y);
    flag2 = (*x1>=0&&*x1<NUMBER_PIXELS_X&&*y1>=0&&*y1<NUMBER_PIXELS_Y);
    if(!flag1 && !flag2)
        return 0;
    if(!flag1 && flag2){
        float temp = *x0;
        *x0 = *x1;
        *x1 = temp;
        temp = *y0;
        *y0 = *y1;
        *y1 = temp;
    }
    //Now x0,y0 is valid and x1,y1 is invalid.
    *y0 = NUMBER_PIXELS_Y - *y0;
    *y1 = NUMBER_PIXELS_Y - *y1; //for mathematical convenience

    //clipping code yay!
    if(round(*x0)==round(*x1)){ //vertical line and y1 is out of bounds
        if(*y1 >= NUMBER_PIXELS_Y)
            *y1 = NUMBER_PIXELS_Y-.5;
        else
            *y1 = 0;
    }
    else{
        float m = (*y1-*y0)/(*x1-*x0);
        if(fabs(m) >= 1){ //step y
            int inc = *y0<*y1 ? 1 : -1;
            float x,y;
            x = 0;
            for(y = *y0; fabs(*y1-y) > .5  && x>=0 && x < NUMBER_PIXELS_X && y >= 0 && y < NUMBER_PIXELS_Y; y += inc){
                float t = (y-*y0)/(*y1-*y0);
                x = *x0 + t*(*x1-*x0);
            }
            *x1 = x;
            *y1 = y;
        }
        else{ //step x
            int inc = *x0<*x1 ? 1 : -1;
            float x,y;
            y = 0;
            for(x = *x0; fabs(*x1-x) > .5 && x>=0 && x < NUMBER_PIXELS_X && y >= 0 && y < NUMBER_PIXELS_Y; x += inc){
                float t = (x-*x0)/(*x1-*x0);
                y = *y0 + t*(*y1-*y0);
            }
            *x1 = x;
            *y1 = y;
        }
    }
    *y0 = NUMBER_PIXELS_Y - *y0;
    *y1 = NUMBER_PIXELS_Y - *y1; //returns the values to screen coordinates
    if(*y1>=NUMBER_PIXELS_Y)
        *y1 = NUMBER_PIXELS_Y-.5;
    return 1;
}


void DrawString(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, char* str, int x, int y) {
    int index = 0;
    while (str[index]) {
        DrawLetter(screen, R, G, B, str[index], x + CHAR_WIDTH*index, y);
        index++;
    }
}

void DrawLetter(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, char c, int x, int y) {
    //x, y is top left corner
    int cur_x, cur_y;
    for (cur_y = 0; cur_y < CHAR_HEIGHT && cur_y + y < HEIGHT; cur_y++) {
        for (cur_x = 0; cur_x < 8 && cur_x + x < WIDTH; cur_x++) {
            if (charMap[c-31][cur_y][cur_x]) {
                DrawPixel(screen, R, G, B, x+cur_x, y+cur_y);
            }
        }
    }
}

void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y) {
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            return;
        }
    }
    switch (screen->format->BytesPerPixel) {
        case 1: { /* Assuming 8-bpp */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *bufp = color;
        }
        break;

        case 2: { /* Probably 15-bpp or 16-bpp */
            Uint16 *bufp;

            bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
            *bufp = color;
        }
        break;

        case 3: { /* Slow 24-bpp mode, usually not used */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *(bufp+screen->format->Rshift/8) = R;
            *(bufp+screen->format->Gshift/8) = G;
            *(bufp+screen->format->Bshift/8) = B;
        }
        break;

        case 4: { /* Probably 32-bpp */
            Uint32 *bufp;

            bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
            *bufp = color;
        }
        break;
    }
    if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }
    //SDL_UpdateRect(screen, x, y, 1, 1);
}
