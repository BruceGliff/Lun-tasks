#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <unistd.h>

#include <sys/wait.h>
#include <pthread.h>


void * func(int * index)
{
    int n = *(int*)(index);
    int k = *(int*)(index+1);

    int b = 0;
    for (b; b < 10; b++)
    {
        index[2]++;
        printf("%d\n", *(index+2));
    }
    
    //printf("*");
    return NULL;
}

       
int main(int argc, char *argv[])
{
        int val = atoi(argv[1]);
        //printf("%d", val);
        val = 10;
        int n = 100;
        int (args)[3];
        int i = 0;
        args[0] = val;
        args[2] = i;
        args[1] = n;

        int b = 0;
        for (b = 0; b < 10; b++)
        {
            pthread_t thread;
            pthread_create(&thread, NULL,
                              func, args);
        }
        sleep(1);
        printf("%d", args[2]);

        return 0;
}
