#ifndef CL_ARGS_H
#define CL_ARGS_H

struct cl_args
{
    unsigned width;
    unsigned height;
    unsigned threads;
    unsigned long iterations;
};


typedef struct cl_args Args;

void parseClArguments(Args* args);

#endif