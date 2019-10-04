#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char * argv[])
{
    if (argc == 1)
        return 0;

    char *endptr;
    int messKey = msgget(IPC_PRIVATE, IPC_CREAT); 
    int n = strtol(argv[1], &endptr, 10);

    int i = 0;
    for (i = 0; i < n; i++)
    {
        printf("%d\n", i);
    }

      
}
