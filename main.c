
#include "clargs.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <complex.h>
#include <SDL.h>

unsigned MAX_ITERATIONS = 8000;
const long double zoom = 2.0;
long double x_lower_bound = -2.0;
long double x_upper_bound = 2.0;
long double y_lower_bound = -2.0;
long double y_upper_bound = 2.0;


long double map(long double x, long double in_min, long double in_max, long double out_min, long double out_max);
int rgb(int red,int green, int blue);
void render_chunk(SDL_Rect *rect,unsigned* px,unsigned width,unsigned height);



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
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
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
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event Event;
    bool running=true,draw=true;
    
    unsigned *px= (unsigned*) malloc(args.width*args.height*sizeof(unsigned));
    if (px==NULL)
    {
        puts("Failed to allocate memory");
        goto clean_up;    
    }

    while (running)
    {
        while(SDL_PollEvent(&Event))
        {
            switch (Event.type)
            {
                case SDL_QUIT:
				    running = false;
				    break;
                
                case SDL_MOUSEBUTTONDOWN:
                {
                    int mouse_x, mouse_y;
                    if (SDL_GetMouseState(&mouse_x, &mouse_y)&SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        long double mp_mouse_x = map(mouse_x, 0, args.width, x_lower_bound, x_upper_bound);
                        long double mp_mouse_y = map(mouse_y, 0, args.height, y_lower_bound, y_upper_bound);
                        long double x_axis_bias = x_upper_bound - (x_upper_bound + x_lower_bound) / 2;
                        long double y_axis_bias = y_upper_bound - (y_upper_bound + y_lower_bound) / 2;
                        x_upper_bound = mp_mouse_x + x_axis_bias;
                        x_lower_bound = mp_mouse_x - x_axis_bias;
                        y_upper_bound = mp_mouse_y + y_axis_bias;
                        y_lower_bound = mp_mouse_y - y_axis_bias;
                        draw = true;
                        puts("Moving the zoom point!");
                        
                    }

                    break;

                }

                case SDL_KEYDOWN:

                    switch(Event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            draw = true;
                            x_lower_bound = -2.0;
                            x_upper_bound = 2.0;
                            y_lower_bound = -2.0;
                            y_upper_bound = 2.0;
                            puts("Reset!");
                            break;

                        case SDLK_r:
                            draw = true;
                            puts("Redrawing!");
                            break;

                        case SDLK_z:
                        {
                            draw = true;
                            long double center_x = (x_lower_bound + x_upper_bound) / 2;
                            long double center_y = (y_lower_bound + y_upper_bound) / 2;

                            x_lower_bound /= zoom;
                            x_upper_bound /= zoom;
                            y_lower_bound /= zoom;
                            y_upper_bound /= zoom;

                            long double x_axis_bias = x_upper_bound - (x_upper_bound + x_lower_bound) / 2;
                            long double y_axis_bias = y_upper_bound - (y_upper_bound + y_lower_bound) / 2;

                            x_upper_bound = center_x + x_axis_bias;
                            x_lower_bound = center_x - x_axis_bias;
                            y_upper_bound = center_y + y_axis_bias;
                            y_lower_bound = center_y - y_axis_bias;

                            puts("Zooming in!");
                            break;
                        }
                    }

                    break;
                

            }
        }

        if (draw)
        {
            puts("Drawing");

            SDL_Rect chunk=
            {
            .x=0,
            .y=0,
            .w=args.width,
            .h=args.height
            };

            // TO DO: Make threaded rendering
            render_chunk(&chunk,px,args.width,args.height);
			
            
            SDL_UpdateTexture(texture,NULL,px,sizeof(unsigned)*args.width);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);

            
            draw=false;
        }

    }

    free(px);

clean_up:

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


long double map(long double x, long double in_min, long double in_max, long double out_min, long double out_max)
{
	// maps value x from [in_min,in_max] range to [out_min,out_max] range
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


int rgb(int red,int green, int blue) { 
	return (red << 16) | (green << 8) | (blue);
}

void render_chunk(SDL_Rect *rect,unsigned* px,unsigned width,unsigned height)
{
    SDL_Event Event;
    for (unsigned y=0; y<rect->h; ++y)
        for (unsigned x=0; x<rect->w; ++x)
        {
            long double cr=map(x+rect->x,0,width,x_lower_bound,x_upper_bound);
            long double ci=map(y+rect->y,0,height,y_lower_bound,y_upper_bound);
            long double complex c=cr+ci*I;

            long double complex z=0+0*I;
            
            unsigned iters=0;
            for (unsigned i=0; i<MAX_ITERATIONS; ++i)
            {
                long double zr=creall(z),zi=cimagl(z);

                // squared norm
                if (zr*zr+zi*zi > 4.0) break;

                z=z*z+c;
                ++iters;

                // window freezes if events aren't pooled for a long time
                if (iters % 4000 == 0)
                    SDL_PollEvent(&Event);

                int r,g,b;
                if (iters==MAX_ITERATIONS)
                    r=g=b=0;
                else
                {
                    g = (iters / 4) % 256;
				    b = (iters + 30) % 256;
				    r = (g + b) / 2;
                }
                
                px[(y + rect->y)*width + x + rect->x] = rgb(r, g, b);


            }



            
        }
}

