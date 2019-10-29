#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

int compare(int fd1, int fd2);

int main()
{
    int fd1 = open("1", O_WRONLY);
    sleep(20);
    int fd2 = open("1", O_RDONLY);

    assert(fd1 >= 0);assert(fd2 >= 0);
    printf("%d\n", compare(fd1, fd2));

    return 0;
}

int compare(int fd1, int fd2)
{
    struct stat stat1;
    struct stat stat2;
    struct stat stat1_;

    int status = 0;

    again:
    status = fstat(fd1, &stat1);
    if (status == -1)
        return -1;

    status = fstat(fd2, &stat2);
    if (status == -1)
        return -1;
    
    status = fstat(fd1, &stat1_);
    if (status == -1)
        return -1;

    if (stat1_.st_ino != stat1.st_ino || stat1_.st_rdev != stat1.st_rdev)
        goto again;

    if (stat2.st_ino != stat1.st_ino || stat2.st_rdev != stat1.st_rdev)
        return 0;

    

    return 1;

}
