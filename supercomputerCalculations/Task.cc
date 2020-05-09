#include <stdlib.h>
#include <string.h>
#include<iostream>
#include <unistd.h>
#include <stdio.h>

#include "Task.h"


#define ERROR(a)    \
    {               \
        perror(a);  \
        exit(-1);   \
    }

TasksQueue::TasksQueue(int fd) : pipe_fd(fd)
{
    tasks_count = 2000;
    in_queue = tasks_count;
    begin = -10e6;
    end   = 10e6;
    del = (end - begin) / tasks_count;

    pthread_mutex_init(&m_res, NULL);
    pthread_mutex_init(&m_pop, NULL);

    for (int i = 0; i != tasks_count; ++i)
    {
        double b = begin + del * i;
        double e = b + del;
        Task t = {begin + del * i, begin + del * (i + 1), 5e-5};
        tasks.push_back(t);
        queue.push_back(i);
    }
}

int TasksQueue::getTask(Task * task)
{
    pthread_mutex_lock(&m_pop);

    if (queue.empty())
    {
        if (in_work.empty())
        {   
            if (pipe_fd == 0)
            {
                pthread_mutex_unlock(&m_pop);
                return 1;
            }
            int rsize = result.size();
            double res = 0;
            for (int i = 0; i != rsize; ++i)
                res += result[i];

            int a = write(pipe_fd, &res, sizeof(double));
            pipe_fd = 0;
            if (a != sizeof(double))
                ERROR("Err final write");
            pthread_mutex_unlock(&m_pop);
            return 1;
        }
        task->del = 0;
        pthread_mutex_unlock(&m_pop);
        return 0;
    }
    int t = queue.back();
    in_work.push_front(t);
    tasks[t].it = in_work.begin();
    queue.pop_back();

    memcpy(task, &tasks[t], sizeof(Task));

    pthread_mutex_unlock(&m_pop);

    return 0;
}

void TasksQueue::write_result(double res, std::list<int>::iterator it)
{
    pthread_mutex_lock(&m_res);
    result.push_back(res);
    --in_queue;
    pthread_mutex_unlock(&m_res);
    delete_task(it);
}
void TasksQueue::push_task(std::list<int>::iterator it)
{
    pthread_mutex_lock(&m_pop);

    int t = *it;
    queue.push_back(t);
    in_work.erase(it);

    pthread_mutex_unlock(&m_pop);
}

void TasksQueue::delete_task(std::list<int>::iterator it)
{
    pthread_mutex_lock(&m_pop);
    in_work.erase(it);
    pthread_mutex_unlock(&m_pop);
}
