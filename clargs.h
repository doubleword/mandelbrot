#ifndef H_CL_ARGS_H
#define H_CL_ARGS_H

struct cl_args
{
    unsigned width;
    unsigned height;
    unsigned threads;
};


typedef struct cl_args Args;

void parseClArguments(Args* args,int argc,char *argv[]);

#endif