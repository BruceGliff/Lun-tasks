# pragma once
//#define _GNU_SOURCE 
#include <pthread.h>
#include <sched.h>
#include <stdio.h>

#include "TaskLocal.h"
#include "cpu_optimization.h"

// memory for threads
typedef pthread_t Threads;


// Task to make free thread busy
void* TrashTask(void * finish)
{
    int i = 0;
    while(1) { ++i; }
    return NULL;
}


// Creating thread with param in attr
int threadCreate(Threads * THREAD, Task_local * TASK, int indx, pthread_attr_t * attr)
{
    int const s = pthread_create(&THREAD[indx], attr, integrate, Task_Get(TASK, indx));
    if (s)
    {
        fprintf(stderr, "Cann't create thread\n");
        exit(-1);
    }

    return 0;
}

// Stick thread to specific core
int set_stick_attr(int core_id, pthread_attr_t * attr) 
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    int s = pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), &cpuset);
    if (s)
    {
        fprintf(stderr, "Cann't stick thread to %d core\n", core_id);
        exit(-1);
    }

    return 0;
}
    
// Create Task discription and memory for threads
Threads * Threads_create(Task_local * TASK)
{
    int const realTreads = TASK->threadsCount_ < CPU_info / 2 ? CPU_info / 2 : TASK->threadsCount_;

    Threads * THREAD = (Threads *) calloc (realTreads, sizeof(Threads));
    if(!THREAD)
    {
        fprintf(stderr, "Cann't alloc memory for threads\n");
        exit(-1);
    }
    return THREAD;
}
int Threads_delete(Threads * THREAD) { free(THREAD); return 0;}

// Start calculating task
double launch(Threads * THREAD, Task_local * TASK)
{
    //--------------------------
    //  init pthread parametrs
    pthread_attr_t attr;
    int s = 0;
    s = pthread_attr_init(&attr);
    if (s)
    {
        fprintf(stderr, "Cann't init attr\n");
        exit(-1);
    }

    //--------------------------
    //  chose propreate algorithm
    for (int i = 0; i !=  TASK->threadsCount_; ++i)
    {
        set_stick_attr(i % CPU_info, &attr);
        threadCreate(THREAD, TASK, i, &attr);
    }

    //--------------------------
    //  create trash threads to hyperthread
    for (int i = TASK->threadsCount_; i < CPU_info / 2; ++i)
    {
        set_stick_attr(i, &attr);
        int const s = pthread_create(&THREAD[i], &attr, TrashTask, NULL);
        if (s)
        {
            fprintf(stderr, "Cann't create thread\n");
            exit(-1);
        }
        int const s1 = pthread_detach(THREAD[i]);
        if (s1)
        {
            fprintf(stderr, "Cann't detach thread\n");
            exit(-1);
        }
    }

    
    //--------------------------
    //   waiting till calculations are done
    double res = 0;
    for (int i = 0; i != TASK->threadsCount_; ++i)
    {   
        int const s = pthread_join(THREAD[i], NULL);
        if (s)
        {
            fprintf(stderr, "Cann't join thread\n");
            exit(-1);
        }
        double const * out = (double *)(TASK->allCache_ + i * TASK->cacheSize);
        res += *out;
    }

    //--------------------------
    //  killing trash threads
    for (int i = TASK->threadsCount_; i < CPU_info / 2; ++i)
        pthread_cancel(THREAD[i]);

    return res;
}
