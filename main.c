
#include "clargs.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "rendering.h"
#include <SDL.h>
#include <pthread.h>
#include <time.h>

unsigned MAX_ITERATIONS = 8000;
long double x_lower_bound = -2.0;
long double x_upper_bound = 2.0;
long double y_lower_bound = -2.0;
long double y_upper_bound = 2.0;

static const long double zoom = 2.0;

int main(int argc, char *argv[])
{

    Args args;
    parseClArguments(&args,argc,argv);

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

    pthread_t *tids= (pthread_t*) malloc(args.threads*sizeof(pthread_t));
    if (tids==NULL)
    {
        puts("Failed to allocate memory");
        free(px);
        goto clean_up;    
    }

    struct ThreadArgs *targs= (struct ThreadArgs*) malloc(args.threads*sizeof(struct ThreadArgs));
    if (targs==NULL)
    {
        puts("Failed to allocate memory");
        free(px);
        free(tids);
        goto clean_up;    
    }


    for (unsigned i=0;i<args.threads;++i)
    {
        targs[i].rect.w=args.width;

        if (i==args.threads-1 && args.height%args.threads)
            targs[i].rect.h=args.height/args.threads+args.height%args.threads;
        else
            targs[i].rect.h=args.height/args.threads;

        targs[i].rect.x=0;
        targs[i].rect.y=args.height/args.threads*i;
        targs[i].px=px;
        targs[i].width=args.width;
        targs[i].height=args.height;

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
                    puts("Moving the zoom point!");

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
                        
                        
                    }

                    break;

                }

                case SDL_KEYDOWN:

                    switch(Event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            puts("Reset!");
                            draw = true;
                            x_lower_bound = -2.0;
                            x_upper_bound = 2.0;
                            y_lower_bound = -2.0;
                            y_upper_bound = 2.0;
                            break;

                        case SDLK_r:
                            puts("Redrawing!");
                            draw = true;
                            break;

                        case SDLK_z:
                        {
                            puts("Zooming in!");

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

                            break;
                        }
                        case SDLK_s:
                        {
                            char filename[256];
                            time_t t=time(NULL);

                            if (t!=-1)
                            {
                                struct tm ct;
                                gmtime_r(&t,&ct);
                                strftime(filename,256,"raw_%Y-%m-%d_%H-%M-%S",&ct);
                            }
                            else
                                strncpy(filename,"raw",4);

                            FILE* bitmap=fopen(filename,"wb");
                            if (bitmap==NULL)
                            {
                                perror("Couldn't open a file");
                                break;
                            }
                            fwrite(px,sizeof(unsigned),args.width*args.height,bitmap);
                            fclose(bitmap);
                            printf("Saved raw bitmap in %s\n",filename);

                            break;
                        }
                        case SDLK_i:
                        {
                            printf("Current amount of iterations: %lu\n",MAX_ITERATIONS);
                            printf("Enter amount of iterations: ");
                            if (scanf("%lu",&MAX_ITERATIONS)!=1)
                            {
                                puts("Wrong input!");
                                int c;
                                while ((c=getchar())!='\n' && c!=EOF);
                            }
                            printf("Amount of iterations is set to %lu\n",MAX_ITERATIONS);

                            break;
                        }

                    }

                    break;
                

            }
        }

        if (draw)
        {
            puts("Drawing");

            for (int i=0;i<args.threads;++i)
                pthread_create(tids+i,NULL,render,targs+i);

            for (int i=0;i<args.threads;++i)
                pthread_join(tids[i],NULL);

            SDL_UpdateTexture(texture,NULL,px,sizeof(unsigned)*args.width);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);

            
            draw=false;
        }

    }

    free(px);
    free(tids);
    free(targs);

clean_up:

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


