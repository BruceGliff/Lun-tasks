#pragma once

#include <vector>
#include <pthread.h>

#define TASKS_COUNT 1000

enum EStatus
{
    IN_QUEUE,
    SENDED,
    FINISHED,
    ERROR
};

struct Task
{
    double begin;
    double end;
    double del;
    double res;
};

struct TasksQueue
{
    int tasks_count;
    double begin;
    double end;
    double del;
    std::vector<Task> queue;
    std::vector<double> result;
    pthread_mutex_t m_pop;
    pthread_mutex_t m_res;

    int getTask(Task * task);
    void Write_result(double res);
    TasksQueue();

};