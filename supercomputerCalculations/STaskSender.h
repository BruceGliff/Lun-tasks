#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Task.h"

struct ConnectionSettings
{
    TasksQueue * q;
    int sk;
};

void * TaskSender(void * data);