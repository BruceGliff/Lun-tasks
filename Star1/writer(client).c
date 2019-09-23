#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define EXIT 'z'
//#define EOF 0

#define MAX 256
#define PATH "/home/brucegliff/Code/Lun-tasks/Star1/fifo"



int main(void) 
{
	puts("Client - writing");
	

    char exit = '!';
    char end = '!';

    unsigned char eof = 0;

    FILE * signal;
    int fd = 0;

    FILE * text = fopen("text.txt", "r");
    unsigned char buf[MAX];

    while (exit != EXIT && !feof(text))
    {   
        sleep(1);
        memset(buf, 0, MAX);
        // TODO make thread for checking exit status

        int code = mkfifo(PATH, 0600);

        // if fifo already exists
        if (code == -1)
        {
            continue;
        }

        fd = open(PATH, O_WRONLY);
        puts("Open fifo");

        while(!feof(text))        
        {
            end = fread(buf + 1, 1, MAX - 1, text);
            buf[0] = end;
            write(fd, buf, MAX);
        }
        write(fd, &eof, 1);
    }

    fclose(text);

    if (fd)
    {
        close(fd);
        puts("Succeed!\n");
        return 0;
    }

    puts("EXIT\n");
    return 0;
}