#ifndef H_RENDERING_H
#define H_RENDERING_H

#include <SDL.h>

long double map(long double x, long double in_min, long double in_max, long double out_min, long double out_max);
int rgb(int red,int green, int blue);


struct ThreadArgs
{
    SDL_Rect rect;
    unsigned *px;
    unsigned width;
    unsigned height;
};


void *render(void *args);


#endif