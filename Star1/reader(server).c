#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define EXIT 'z'
#define MAX 256
typedef int bool;

#define PATH "/home/brucegliff/Code/Lun-tasks/Star1/fifo"

#define DEBUG puts("!");fflush(NULL);

int main() 
{
	puts("Server - reading");

    char exit = '!';
    char end = '!';
    FILE * signal;
    int fd = 0;

    bool IsInCharge = 0;
    bool IsPrinted = 0;

    FILE * text = fopen("copyText.txt", "w");
    char buf[MAX];

    while (exit != EXIT)
    {   
        sleep(1);
        memset(buf, 0, MAX);
        // TODO make thread for checking exit status
        if (!IsInCharge)
        {   
            signal = fopen("signal", "r");

            // if fifo already exists
            if (signal != NULL)
            {
                fclose(signal);
                continue;
            }   

            signal = fopen("signal", "w");
            fclose(signal);
        }

        IsInCharge = 1;

        fd = open(PATH, O_RDONLY);
        if (!IsPrinted)
            puts("Try to open fifo");
        
        IsPrinted = 1;

        if (fd == -1)
        {   
            continue;
        }
        puts("Open fifo");
        fflush(NULL);
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

    remove("signal");
    remove("fifo");



    if (fd)
    {
        close(fd);
        puts("Succeed copy!\n");
        return 0;
    }

    puts("EXIT\n");
    return 0;
}