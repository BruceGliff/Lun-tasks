# pragma once
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "Task.h"

#define MAX_THREADS INT_MAX

typedef struct cacheLine
{
    double out;
    double begin;
    double end;
} cacheLine;

typedef struct Task_local
{
    int      cacheSize;
    double   begin;
    double   end;
    double   del;
    char *   allCache_;
    int      threadsCount_;
} Task_local;

Task_local * Task_create(int threadsCount, Task * t)
{
    Task_local * TASK = (Task_local *) malloc (sizeof(Task_local));
    if (!TASK)
    {
        fprintf(stderr, "Can not allocate memory for Task!");
        exit(-1);
    }
    TASK->cacheSize = 128;
    TASK->begin = t->begin;
    TASK->end = t->end;


    TASK->threadsCount_ = threadsCount < MAX_THREADS ? threadsCount : MAX_THREADS; 

    TASK->del = (TASK->end - TASK->begin) / TASK->threadsCount_;
    TASK->allCache_ = (char *) calloc (sizeof(char), TASK->cacheSize * TASK->threadsCount_);
    if (!TASK->allCache_)
    {
        fprintf(stderr, "Can not allocate memory for cache!");
        exit(-1);
    }

    return TASK;
}
    
int Task_delete(Task_local * TASK) { free(TASK->allCache_); free(TASK); return 0; }

void * Task_Get(Task_local * TASK, int i)
{
    cacheLine * line = (cacheLine *)(TASK->allCache_ + i * TASK->cacheSize);
    line->begin = TASK->begin + i * TASK->del;
    line->end = TASK->begin + (i + 1) * TASK->del;
    return (void *) line;
}


void* integrate(void * cache)
{
    double const desc = 10e-4;
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
