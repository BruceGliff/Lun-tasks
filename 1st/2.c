#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc == 2) 
        execlp(argv[1], "", NULL);
    else
    {
        execvp(argv[1], argv + 2);
    }
    


    return 0;
}