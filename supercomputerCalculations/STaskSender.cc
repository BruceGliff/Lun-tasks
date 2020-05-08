#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "STaskSender.h"
#include "Task.h"


#define ERROR(a)    \
    {               \
        perror(a);  \
        exit(-1);   \
    }

void * TaskSender(void * data)
{
    ConnectionSettings * set = (ConnectionSettings *) data;

    int s_fd = set->sk;
    TasksQueue * q = set->q;

    Task t;
    while(!q->getTask(&t))
    {
        puts("send task");
        int res = write(s_fd, &t, sizeof(Task));
        if (res != sizeof(Task))
        {   
            perror("Connection lost");
            if (shutdown(s_fd, SHUT_RDWR) == -1)
                ERROR("Err shutdown sk");
            return NULL;
        }
        double result = 0;
        res = read(s_fd, &result, sizeof(double));
        if (res != sizeof(double))
        {
            perror("Connection lost");
            if (shutdown(s_fd, SHUT_RDWR) == -1)
                ERROR("Err shutdown sk");
            return NULL;
        }

        q->Write_result(result);

        puts("end send task");
    }

    if (shutdown(s_fd, SHUT_RDWR) == -1)
        ERROR("Err shutdown sk");
    
    return NULL;    
}