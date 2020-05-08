#pragma once

#include <vector>
#include <pthread.h>
#include <list>

struct Task
{
    double begin;
    double end;
    double del;
    std::list<int>::iterator it;
};

struct TasksQueue
{
    int tasks_count;
    int pipe_fd;
    double begin;
    double end;
    double del;
    int in_queue;
    std::vector<Task> tasks;
    std::vector<int> queue;
    std::vector<double> result;
    std::list<int> in_work;
    pthread_mutex_t m_pop;
    pthread_mutex_t m_res;

    int getTask(Task * task);
    void push_task(std::list<int>::iterator it);
    void write_result(double res, std::list<int>::iterator it);
    void delete_task(std::list<int>::iterator it);
    TasksQueue(int fd);

};