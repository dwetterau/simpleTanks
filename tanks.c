#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <unistd.h>
#include <math.h>
#include "graphics.h"
#include "charMap.h"

#define PI 3.14159265358979323846
#define EXPLOSION_RAD 10
#define TANK_WIDTH 5
#define TANK_HEIGHT 4
#define BARREL .5

/**
 *
 * Written by David Wetterau
 *
 */

struct color {
   Uint8 red;
   Uint8 green;
   Uint8 blue;
} typedef color;

struct tank {
    int x;
    int y;
    int power;
    float angle;
} typedef tank;

tank tanks [2]; //0 is tank on the left its angle is like the unit circle, 
                //1 has its angle go like unit circle flipped over y axis

color colorMap [5]; //0 = empty, 1 = grass, 2 = tank1, 3 = tank2, 4 = projectile
char gameState [WIDTH*HEIGHT];

void makeColor(color * c, Uint8 r, Uint8 g, Uint8 b) {
    c->red = r;
    c->green = g;
    c->blue = b;
}
float min(float num1, float num2) {
    if (num1 < num2) {
        return num1;
    }
    return num2;
}

float max(float num1, float num2) {
    if (num1 > num2) {
        return num1;
    }
    return num2;
}
/**
 * Issues with this:
 * - Tank 1 cannot shoot at angles more than pi/2 rads, aka it can only shoot to the right..
 * - Not easily switchable between tank1 and 2, so I'll just make two different functions
 */
void shootGunTank1(int* poi) {
    // each pixel = 1 meter.
    // We need to turn the equation to use the angle and stuff
    // All we really need in initial velocity in the y direction
    // max power = 100 m/s
    // x(t) = v*cos(ang)*t
    // y(t) = v*sin(ang)*t - (1/2)*g*t^2
    // t = x/(v*cos(ang))
    int p = tanks[0].power;
    float cos_ap = p*cos(tanks[0].angle);
    float sin_ap = p*sin(tanks[0].angle);

    int collidingWith = 0;
    int x = tanks[0].x;
    int y = tanks[0].y;   
    int base_y = y;
    int base_x = x;
                                                //remember 2 = tank1, 3 = tank2
    while ((collidingWith == 0 || collidingWith == 2 || collidingWith == 4) 
        && x < WIDTH-1 && y > 0) {
        x++;
        float t = (x-base_x)/(cos_ap);
        int new_y = (int)(sin_ap*t - .5 * 9.81 * t * t) + base_y;
        //look above to see if we ran into anything...
        int check_y;
        int inc;
        if (new_y < y) {
            inc = -1;
        } else {
            inc = 1;
        }
        for (check_y = y; check_y != new_y; check_y += inc) {
            if (check_y < HEIGHT) {
                int temp = gameState[(HEIGHT-check_y)*WIDTH + x]; 
                gameState[(HEIGHT-check_y)*WIDTH + x] = 4; //projectile at this point.
                if (temp != 0 && temp != 2) {
                    check_y += inc;
                    break;
                }
            }
        }
        y = check_y;
        if (y < HEIGHT) {
            collidingWith = gameState[(HEIGHT-y)*WIDTH + x];
            if (new_y == y) {
                gameState[(HEIGHT-y)*WIDTH + x] = 4; //projectile at this point.
            }
        }
    }
    *poi = x;
    *++poi = y;
}

/**
 * Issues with this:
 * - Tank 2 cannot shoot at angles more than pi/2 rads, aka it can only shoot to the left..
 */
void shootGunTank2(int* poi) {
    // each pixel = 1 meter.
    // We need to turn the equation to use the angle and stuff
    // All we really need in initial velocity in the y direction
    // max power = 100 m/s
    // x(t) = v*cos(ang)*t
    // y(t) = v*sin(ang)*t - (1/2)*g*t^2
    // t = x/(v*cos(ang))
    int p = tanks[1].power;
    float cos_ap = p*cos(tanks[1].angle);
    float sin_ap = p*sin(tanks[1].angle);

    int collidingWith = 0;
    int x = tanks[1].x;
    int y = tanks[1].y;   
    int base_y = y;
    int base_x = x;
    x = 0;
                                                //remember 2 = tank1, 3 = tank2
    while ((collidingWith == 0 || collidingWith == 3 || collidingWith == 4) 
            && (base_x - x > 0) && y > 0) {
        x++;
        float t = x/(cos_ap);
        int new_y = (int)(sin_ap*t - .5 * 9.81 * t * t) + base_y;
        //look above to see if we ran into anything...
        int check_y;
        int inc;
        if (new_y < y) {
            inc = -1;
        } else {
            inc = 1;
        }
        for (check_y = y; check_y != new_y; check_y += inc) {
            if (check_y < HEIGHT) {
                int temp = gameState[(HEIGHT-check_y)*WIDTH + (base_x-x)]; 
                gameState[(HEIGHT-check_y)*WIDTH + (base_x-x)] = 4; 
                if (temp != 0 && temp != 3) {
                    check_y += inc;
                    break;
                }
            }
        }
        y = check_y;
        if (y < HEIGHT) {
            collidingWith = gameState[(HEIGHT-y)*WIDTH + (base_x-x)];
            if (new_y == y) {
                gameState[(HEIGHT-y)*WIDTH + (base_x-x)] = 4; //projectile at this point.
            }
        }
    }
    *poi = base_x - x;
    *++poi = y;
}

void initializeTanks() {
    tank* t = &tanks[0];
    t->x = 100;
    t->y = 300;
    t->power = 50;
    t->angle = PI/4;
    t = &tanks[1];
    t->x = 750;
    t->y = 300;
    t->power = 50;
    t->angle = PI/4;
}

void initializeGame() {
    initializeTanks();
    //For now I'm just going to make it a hilly thing:
    // f(x) = 100*sin(x/(50)*pi) + 300
    int x, y;
    for (x = 0; x < WIDTH; x++) {
        int start = (int) (30.0*sin(x/50.0*PI)) + 300;
        for (y = start; y < HEIGHT; y++) {
            gameState[y*WIDTH+x] = 1;
        }
    }
}

float dist(int x1, int y1, int x2, int y2) {
    return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

/**
 *
 * Returns the number of the tank hit or -1 otherwise
 *
 */
int makeCrater(int x, int y) {
    //make bounding box
    int x_min = max(x-EXPLOSION_RAD, 0);
    int x_max = min(x+EXPLOSION_RAD, WIDTH-1);
    int y_min = max(y-EXPLOSION_RAD, 0);
    int y_max = min(y+EXPLOSION_RAD, HEIGHT-1);
    int cur_x, cur_y, to_return;
    to_return = -1;
    for (cur_x = x_min; cur_x <= x_max; cur_x++) {
        for (cur_y = y_min; cur_y <= y_max; cur_y++) {
            if (dist(cur_x, cur_y, x, y) <= EXPLOSION_RAD) {
                int index = (HEIGHT-cur_y)*WIDTH + cur_x;
                if (gameState[index] == 2 || gameState[index] == 3) {
                    to_return = gameState[index] - 2; //tank number
                }
                gameState[index] = 0;
            }
        }
    }
    return -1;
}

//TODO make this less COMPLETELY shitty
void clearShot() {
    int x, y;
    for (x = 0; x < WIDTH; x++) {
        for (y = 0; y < HEIGHT; y++) {
            if(gameState[y*WIDTH + x] == 4) {
                gameState[y*WIDTH + x] = 0;
            }
        }
    }
}

void placeTank(int index) {
    int x_min, x_max, y_min, y_max, x, y;
    x_min = max(0, tanks[index].x - TANK_WIDTH);
    x_max = min(WIDTH-1, tanks[index].x + TANK_WIDTH);
    y_min = max(0, tanks[index].y - TANK_HEIGHT);
    y_max = min(HEIGHT-1, tanks[index].y + TANK_HEIGHT);
    for (x = x_min; x <= x_max; x++) {
        for (y = y_min; y <= y_max; y++) {
            gameState[(HEIGHT - y)*WIDTH + x] = 2 + index;
        }
    }
}

void placeTanks() {
    //TODO make sure there's no hole beneath the tank now and move it down
    placeTank(0);
    placeTank(1);
}

void drawBarrel(int tankNumber, SDL_Surface *screen) {;
    int x_barrel_end;
    if (tankNumber == 0) {
        x_barrel_end = tanks[tankNumber].power * BARREL * cos(tanks[tankNumber].angle) + tanks[tankNumber].x;
    } else {
        x_barrel_end = - tanks[tankNumber].power * BARREL * cos(tanks[tankNumber].angle) + tanks[tankNumber].x;
    }
    int y_barrel_end = -tanks[tankNumber].power * BARREL * sin(tanks[tankNumber].angle) + tanks[tankNumber].y;
   color * col = &colorMap[2+tankNumber];
   DrawLine(screen, col->red, col->green, col->blue, tanks[tankNumber].x, 
   HEIGHT - tanks[tankNumber].y, x_barrel_end, HEIGHT - y_barrel_end);
}

void drawGame(SDL_Surface *screen) {
    int x, y;
    for (x = 0; x < WIDTH; x++) {
        for (y = 0; y < HEIGHT; y++) {
            color * col = &colorMap[gameState[y*WIDTH + x]];
            DrawPixel(screen, col->red, col->green, col->blue, x, y);
        }
    }
    drawBarrel(0, screen);
    drawBarrel(1, screen);
}

void drawOverlay(SDL_Surface *screen) {
    DrawString(screen, 255, 0, 0, "Tanks by: David & Adam", 0, 0);
}

int main() {
//FILL COLOR MAP===============================================================
    makeColor(&colorMap[0], 0, 0, 0);
    makeColor(&colorMap[1], 0, 255, 0);
    makeColor(&colorMap[2], 255, 0, 0);
    makeColor(&colorMap[3], 0, 0, 255);
    makeColor(&colorMap[4], 255, 255, 255);
//INITIALIZE GAME==============================================================
    initializeGame();
    /*int poi [2];
    shootGunTank1(poi);
    printf("x: %d, y: %d\n", poi[0], poi[1]);
    int res = makeCrater(poi[0], poi[1]);
    printf("Hit tank: %d\n", res);
    
    shootGunTank2(poi);
    printf("x: %d, y: %d\n", poi[0], poi[1]);
    res = makeCrater(poi[0], poi[1]);
    printf("Hit tank: %d\n", res);
    */
    //clearShot();
//WINDOW FRAME SETUP===========================================================
    SDL_Surface *screen;
    screen = SDL_SetVideoMode(WIDTH,HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if ( screen == NULL ) {
        fprintf(stderr, "Unable to set WIDTHxHEIGHT video: %s\n", SDL_GetError());
        exit(1);
    }
//MAIN ANIMATION LOOP==========================================================
    int quit = 0;
    SDL_Event event; 
    int poi [2];
    int res;
    float c = 0;
    int start = SDL_GetTicks();
    int madeShot1, madeShot2;
    while(!quit){
        placeTanks();
        drawGame(screen);
        drawOverlay(screen);
        if (madeShot1) {
            clearShot();
            madeShot1 = 0;
        }
        if (madeShot2) {
            clearShot();
            madeShot2 = 0;
        }

        SDL_Flip(screen);
        SDL_FillRect(screen,NULL,0);
        //Handle screen events...
        SDL_PollEvent(&event);
        switch(event.type){
            case SDL_QUIT:
                quit = 1;
                SDL_Quit();
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = 1;
                        SDL_Quit();
                        break;
                    //Tank 1 stuff
                    case SDLK_a:
                        //tilt tank1 gun up
                        tanks[0].angle = min(PI/2-.01, tanks[0].angle + PI/200);
                        break;
                    case SDLK_d:
                        //tilt tank1 gun down
                        tanks[0].angle = max(0, tanks[0].angle - PI/200);
                        break;
                    case SDLK_w:
                        tanks[0].power = min(200, tanks[0].power+1);
                        break;
                    case SDLK_s:
                        tanks[0].power = max(1, tanks[0].power-1);
                        break;
                    case SDLK_LSHIFT:
                        //fire shot
                        if (!madeShot1) {
                            shootGunTank1(poi);
                            res = makeCrater(poi[0], poi[1]);
                            madeShot1 = 1;
                        }
                        break;
                    //Tank 2 stuff
                    case SDLK_l:
                        //tilt tank1 gun up
                        tanks[1].angle = min(PI/2-.01, tanks[1].angle + PI/200);
                        break;
                    case SDLK_j:
                        //tilt tank1 gun down
                        tanks[1].angle = max(0, tanks[1].angle - PI/200);
                        break;
                    case SDLK_i:
                        tanks[1].power = min(200, tanks[1].power+1);
                        break;
                    case SDLK_k:
                        tanks[1].power = max(1, tanks[1].power-1);
                        break;
                    case SDLK_RSHIFT:
                        //fire shot
                        if (!madeShot2) {
                            shootGunTank2(poi);
                            res = makeCrater(poi[0], poi[1]);
                            madeShot2 = 1;
                        }
                        break;
                    default: 
                        break;
                }
                break;
            default:
                break;
        }
        //We want to show the animation
        SDL_Delay(10);
        c+=.01;
        //printf("FPS: %f\n",(c*100)/((SDL_GetTicks()-start)/1000));
    }
return 0;
}

