#include "clargs.h"
#include <stdlib.h>
#include <stdio.h>


const char *UsageStr=
"Usage: %s width height threads\n\n"
"width\t\t\tWidth of the window in pixels\n"
"height\t\t\tHeight of the window in pixels\n"
"threads\t\t\tAmount of threads to use for rendering\n\n"
"Controls:\n\n"
"Left-click\t\tChange the zoom point\n"
"Z\t\t\tZoom in\n"
"R\t\t\tRedraw\n"
"ESC\t\t\tReset\n"
"S\t\t\tSave bitmap (can be converted in a png image with the raw2png.py script)\n"
"I\t\t\tChange the amount of iterations used to decide whether a point lies in the set\n\n";



void parseClArguments(Args* args,int argc,char *argv[])
{


    if (argc<4)
    {
        printf(UsageStr,argv[0]);
        exit(1);
    }

    args->width=(unsigned) strtoul(argv[1],NULL,10);
    if (!args->width)
    {
        puts("Invalid width value");
        exit(1);
    }

    args->height=(unsigned) strtoul(argv[2],NULL,10);
    if (!args->height)
    {
        puts("Invalid height value");
        exit(1);
    }

    args->threads=(unsigned) strtoul(argv[3],NULL,10);
    if (!args->threads)
    {
        puts("Invalid threads value");
        exit(1);
    }



};