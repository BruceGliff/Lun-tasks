#include "Thread.h"


int main()
{
    int const workingThreads = inputParser(argc, argv[1]);
    GetCpuConfiguration();
    Task_local * TASK = Task_create(CPU_info);
    Threads * THREAD =  Threads_create(TASK);
    printf("%e\n", launch(THREAD, TASK));

    Threads_delete(THREAD);
    Task_delete(TASK);
    return 0;
}
