# pragma once
#include <stdlib.h>
#include <stdio.h>

#define MAX_THREADS 144

typedef struct cacheLine
{
    double out;
    double begin;
    double end;
} cacheLine;

struct Task
{
    int      cacheSize;
    double   begin;
    double   end;
    double   del;
    char *   allCache_;
    int      threadsCount_;
} TASK;

int Task_create(int threadsCount)
{
    TASK.cacheSize = 128;
    TASK.begin = -5e3;
    TASK.end = 5e3;


    TASK.threadsCount_ = threadsCount < MAX_THREADS ? threadsCount : MAX_THREADS; 

    TASK.del = (TASK.end - TASK.begin) / TASK.threadsCount_;
    TASK.allCache_ = (char *) calloc (sizeof(char), TASK.cacheSize * TASK.threadsCount_);
    if (!TASK.allCache_)
    {
        fprintf(stderr, "Can not allocate memory for cache!");
        exit(-1);
    }

    return 0;
}
    
int Task_delete() { free(TASK.allCache_); return 0; }

void * Task_Get(int i)
{
    cacheLine * line = (cacheLine *)(TASK.allCache_ + i * TASK.cacheSize);
    line->begin = TASK.begin + i * TASK.del;
    line->end = TASK.begin + (i + 1) * TASK.del;
    return (void *) line;
}


void* integrate(void * cache)
{
    double const desc = 10e-6;
    cacheLine * out = (cacheLine *)cache;
    double sum = 0;
    while (out->begin <= out->end)
    {
        sum += out->begin * out->begin * desc;
        out->begin += desc;
    }
    out->out = sum;

    return NULL;
}