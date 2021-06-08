#include "rendering.h"
#include <complex.h>


extern long double x_lower_bound;
extern long double x_upper_bound;
extern long double y_lower_bound;
extern long double y_upper_bound;
extern unsigned MAX_ITERATIONS;


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



