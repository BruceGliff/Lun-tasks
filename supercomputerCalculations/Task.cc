#include <stdlib.h>
#include <string.h>
#include<iostream>

#include "Task.h"

TasksQueue::TasksQueue()
{
    tasks_count = 1000;
    begin = -10e6;
    end   = 10e6;
    del = (end - begin) / tasks_count;

    pthread_mutex_init(&m_res, NULL);
    pthread_mutex_init(&m_pop, NULL);

    for (int i = 0; i != tasks_count; ++i)
    {
        double b = begin + del * i;
        double e = b + del;
        Task t = {begin + del * i, begin + del * (i + 1), 10e-6, 0};
        queue.push_back(t);
    }
}

int TasksQueue::getTask(Task * task)
{
    pthread_mutex_lock(&m_pop);

    if (queue.empty())  
        return 1;

    Task t = queue.back();
    queue.pop_back();

    memcpy(task, &t, sizeof(Task));

    pthread_mutex_unlock(&m_pop);

    return 0;
}

void TasksQueue::Write_result(double res)
{
    pthread_mutex_lock(&m_res);
    result.push_back(res);
    pthread_mutex_unlock(&m_res);
}
