# pragma once
#define _GNU_SOURCE 
#include <pthread.h>
#include <sched.h>
#include <sys/sem.h>

#include "Task.h"
#include "cpu_optimization.h"

struct semaphore
{
    int sem_id;
    struct sembuf set0_N;
    struct sembuf minus1;
} SEM;

// memory for threads
struct Threads
{
    pthread_t * threads;
} THREAD;

int semaphore_set()
{
    key_t const key = ftok("/tmp", 'a');
    SEM.sem_id = semget(key, 6, IPC_CREAT | IPC_EXCL | 0666);
	if (SEM.sem_id == -1)
		SEM.sem_id = semget(key, 6, 0666);	

	SEM.set0_N.sem_num = 0;
    SEM.set0_N.sem_flg = 0;
    SEM.set0_N.sem_op = 0;

    SEM.minus1.sem_num = 0;
    SEM.minus1.sem_flg = IPC_NOWAIT;
    SEM.minus1.sem_op = -1;

    return SEM.sem_id;
}

int semaphore_setN()
{
	SEM.set0_N.sem_num = 0;
    SEM.set0_N.sem_flg = IPC_NOWAIT;
    SEM.set0_N.sem_op = CPU_info / 2 - TASK.threadsCount_;

    return 0;
}

// Task to make free thread busy
void* TrashTask(void * finish)
{
    while(semop(SEM.sem_id, &SEM.minus1, 1) == -1) {}

    return NULL;
}


// Creating thread with param in attr
int threadCreate(int indx, pthread_attr_t * attr)
{
    int const s = pthread_create(&THREAD.threads[indx], attr, integrate, Task_Get(indx));
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
int Threads_create(int threadsCount)
{
    Task_create(threadsCount);
    int const realTreads = TASK.threadsCount_ < CPU_info / 2 ? CPU_info / 2 : TASK.threadsCount_;

    THREAD.threads = (pthread_t *) calloc (realTreads, sizeof(pthread_t));
}
int Threads_delete() { free(THREAD.threads); Task_delete(); }

// Start calculating task
double launch()
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
    for (int i = 0; i !=  TASK.threadsCount_; ++i)
    {
        set_stick_attr(i % CPU_info, &attr);
        threadCreate(i, &attr);
    }


    //--------------------------
    //  create trash threads to hyperthread
    //  setup a semaphore
    semop(semaphore_set(), &SEM.set0_N, 1);

    for (int i = TASK.threadsCount_; i < CPU_info / 2; ++i)
    {
        set_stick_attr(i, &attr);
        int const s = pthread_create(&THREAD.threads[i], &attr, TrashTask, NULL);
        if (s)
        {
            fprintf(stderr, "Cann't create thread\n");
            exit(-1);
        }
        int const s1 = pthread_detach(THREAD.threads[i]);
        if (s1)
        {
            fprintf(stderr, "Cann't detach thread\n");
            exit(-1);
        }
    }

    
    //--------------------------
    //   waiting till calculations are done
    double res = 0;
    for (int i = 0; i != TASK.threadsCount_; ++i)
    {   
        int const s = pthread_join(THREAD.threads[i], NULL);
        if (s)
        {
            fprintf(stderr, "Cann't join thread\n");
            exit(-1);
        }
        double const * out = (double *)(TASK.allCache_ + i * TASK.cacheSize);
        res += *out;
    }

    //--------------------------
    //  waiting till trash is done
    semaphore_setN();
    semop(SEM.sem_id, &SEM.set0_N, 1);

    return res;
}