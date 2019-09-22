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

int main() 
{
	puts("Server - reading");

    char exit = '!';
    char end = '!';
    FILE * signal;
    int fd = 0;

    FILE * text = fopen("copyText.txt", "w");
    char buf[MAX];

    while (exit != EXIT)
    {   
        memset(buf, 0, MAX);
        // TODO make thread for checking exit status

        signal = fopen("signal", "r");
        //printf("%d\n", signal);
        // if fifo already exists
        if (signal != NULL)
        {
            fclose(signal);
            continue;
        }

        signal = fopen("signal", "w");
        fclose(signal);

        fd = open(PATH, O_RDONLY);
        puts("Open fifo\n");

        if (fd == -1)
        {
            





        }

        while(1)
        {    
            end = read(fd, buf, MAX);
            unsigned char size = buf[0];

            fwrite(buf+1, 1, size, text);
            fflush(text);
            if (!size)
            {
                exit = EXIT;
                break;
            }
        }
    }

    fclose(text);

    system("rm signal");
    system("rm fifo");



    if (fd)
    {
        close(fd);
        puts("Succeed copy!\n");
        return 0;
    }

    puts("EXIT\n");
    return 0;
}