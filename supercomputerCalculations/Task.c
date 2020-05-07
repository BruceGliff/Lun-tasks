#include <stdlib.h>

#include "Task.h"

struct Task * generateTasksBus()
{
    double const begin = -10e6;
    double const end   = 10e6;


    struct Task * bus = (struct Task *) malloc (TASKS_COUNT * sizeof(struct Task));
    for (int i = 0; i != TASKS_COUNT; ++i)
    {
        
    }
}

struct Task getTask()
{
    static int free = 0; 
}
