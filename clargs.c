#include "clargs.h"
#include <stdio.h>



static void init(Args* args)
{

    args->height=600;
    args->width=800;
    args->threads=2;
    args->iterations=8000;

};

void parseClArguments(Args* args)
{
    init(args);
};