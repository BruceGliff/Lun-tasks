#include <unistd.h>


#include "STaskSender.h"


#define ERROR(a)    \
    {               \
        perror(a);  \
        exit(-1);   \
    }

void * TaskSender(void * data)
{
    int s_fd = *((int *) data);

    

}