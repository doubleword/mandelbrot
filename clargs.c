#include "clargs.h"
#include <stdlib.h>
#include <stdio.h>


const char *UsageStr=
"Usage: %s width height threads\n\n"
"width\t\tWidth of the windows in pixels\n"
"height\t\tHeight of the windows in pixels\n"
"threads\t\tAmount of threads to use for rendering\n";



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