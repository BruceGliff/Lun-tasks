#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>

int main()
{
	printf("%d\n", getpid());
	int a = mkfifo("1.tr", 0600);
	int fd = open("1.tr", O_WRONLY | O_NONBLOCK, 0600);
	fcntl(fd, F_SETFL, O_ASYNC);
	fcntl(fd, F_SETOWN, getpid());
	

	sleep(10);
}
