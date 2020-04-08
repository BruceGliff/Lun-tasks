#include "Thread.h"

int inputParser(int argc, char * argv);


int main(int argc, char * argv[])
{
    printf("%d", GetCpuConfiguration());
    return 0;
}


int inputParser(int argc, char * argv)
{
    if(argc == 1)
        return 1;
    else
        return strtol(argv, NULL, 10);
}