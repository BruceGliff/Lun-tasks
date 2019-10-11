#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>

int compare(int fd1, int fd2);

int main()
{
    int fd1 = open("xx", O_WRONLY | O_CREAT);
    int fd2 = open("xx", O_RDONLY);

    assert(fd1 >= 0 && fd2 >= 0);
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
