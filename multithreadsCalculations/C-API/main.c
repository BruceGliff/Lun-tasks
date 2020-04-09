#include "Thread.h"

int inputParser(int argc, char * argv);


int main(int argc, char * argv[])
{
    int const workingThreads = inputParser(argc, argv[1]);
    GetCpuConfiguration();
    Task * TASK = Task_create(workingThreads);
    Threads * THREAD =  Threads_create(TASK);
    printf("%e\n", launch(THREAD, TASK));

    Threads_delete(THREAD);
    Task_delete(TASK);
    return 0;
}


int inputParser(int argc, char * argv)
{
    if(argc == 1)
        return 1;
    else
        return strtol(argv, NULL, 10);
}