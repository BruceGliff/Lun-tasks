#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        if (t.del == 0)
            continue;

        int task_num = *t.it;
        printf("Send task [%4d] -------------v\n", task_num);
        int res = write(s_fd, &t, sizeof(Task));
        if (res != sizeof(Task))
        {   
            perror("Connection lost write");
            q->push_task(t.it);
                    
            printf("Task will [%4d] be resended -^\n", task_num);
            if (shutdown(s_fd, SHUT_RDWR) == -1)
                ERROR("Err shutdown sk");
            return NULL;
        }
        double result = 0;
        res = read(s_fd, &result, sizeof(double));
        if (res != sizeof(double))
        {
            perror("Connection lost read");
            q->push_task(t.it);
            printf("Task will [%4d] be resended -^\n", task_num);
            if (shutdown(s_fd, SHUT_RDWR) == -1)
                ERROR("Err shutdown sk");
            return NULL;
        }

        q->write_result(result, t.it);

        printf("Task done [%4d] -------------^\n", task_num);
    }

    if (shutdown(s_fd, SHUT_RDWR) == -1)
        ERROR("Err shutdown sk");
    
    return NULL;    
}