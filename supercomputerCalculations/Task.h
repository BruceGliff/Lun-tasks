#pragma once

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
    EStatus status;
};


struct Task getTask();
struct Task * generateTasksBus();