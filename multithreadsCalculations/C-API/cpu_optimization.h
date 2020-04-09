#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int CPU_info = 0;

int GetCpuConfiguration()
{
    char const * cpuifo = "/sys/devices/system/cpu/online";
    CPU_info = 4;
    int fd = open(cpuifo, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Can not open %s\n. Use only 4 threads!\n", cpuifo);
        return 4;
    }

    char inData[10] = "";
    if(read(fd, inData, 10) == 0)
    {
        close(fd);
        fprintf(stderr, "Can not read from %s\n. Use only 4 threads!\n", cpuifo);
        return 4;
    }
    

    int firstOnlineThread = 0, secondOnlineThread = 0;

    sscanf(inData, "%d-%d", &firstOnlineThread, &secondOnlineThread);
    close(fd);


    CPU_info = secondOnlineThread - firstOnlineThread + 1;
    return CPU_info;
}
