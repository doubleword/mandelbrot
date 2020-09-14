
#include "clargs.h"
#include <stdio.h>
#include <SDL.h>
#include <unistd.h>


int main(int argc, char *argv[])
{

    Args args;
    parseClArguments(&args);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }


    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    window = SDL_CreateWindow(
        "Mandelbrot",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        args.width,args.height,
        0
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("Could not create renderer: %s\n", SDL_GetError());
        return 1;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        args.width,args.height
    );

    if (texture == NULL) {
        printf("Could not create texture: %s\n", SDL_GetError());
        return 1;
    }



    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}