#pragma once

#include <vector>
#include <pthread.h>

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
    int in_queue;
    std::vector<Task> queue;
    std::vector<double> result;
    pthread_mutex_t m_pop;
    pthread_mutex_t m_res;

    int getTask(Task * task);
    void pushTash(Task const & task);
    void Write_result(double res);
    TasksQueue();

};