#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int compare(int fd1, int fd2);

int main()
{
    int fd1 = open("1", O_WRONLY);
    int fd2 = open("/home/brucegliff/Code/Lun-tasks/Star1/FD/1", O_RDONLY);

    printf("%d", compare(fd1, fd2));

    return 0;
}

int compare(int fd1, int fd2)
{
    struct stat stat1;
    struct stat stat2;

    int status = fstat(fd1, &stat1);
    if (status == -1)
        return -1;

    status = fstat(fd2, &stat2);
    if (status == -1)
        return -1;

    // inode unique for each file in one file system

    // devices must be indentical for one file system

    if ((stat2.st_ino == stat1.st_ino) && (stat2.st_dev == stat1.st_dev))
        return 1;

    return 0;

}