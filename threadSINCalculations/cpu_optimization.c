#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "cpu_optimization.h"

int getCpuConfiguration() {
    char const * cpuifo = "/sys/devices/system/cpu/online";
    int CPU_info = 4;
    int fd = open(cpuifo, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Can not open %s\n. Use only 4 threads!\n", cpuifo);
        return CPU_info;
    }

    char inData[10] = "";
    if(read(fd, inData, 10) == 0) {
        close(fd);
        fprintf(stderr, "Can not read from %s\n. Use only 4 threads!\n", cpuifo);
        return CPU_info;
    }
    
    int firstOnlineThread = 0, secondOnlineThread = 0;

    sscanf(inData, "%d-%d", &firstOnlineThread, &secondOnlineThread);
    close(fd);
    
    CPU_info = secondOnlineThread - firstOnlineThread + 1;
    return CPU_info;
}

int getThreadNumber(int argc, const char* argv[]) {
    int tNumber = 0;
	if (argc > 1) {
        tNumber = atoi(argv[1]);
	}
    if (!tNumber) {
        fprintf(stdout, "Number of threads is not specified. Use HW info\n");
		tNumber = getCpuConfiguration();
    }
    fprintf(stdout, "Calculation on %d thread(s)\n", tNumber);

    return tNumber;
}