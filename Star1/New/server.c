#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define EXIT 'z'

#define MAX 256
#define PATH "/home/brucegliff/Code/Lun-tasks/Star1/fifo"



int main(int argc, char * argv[]) 
{
    FILE * text = NULL;
    switch (argc)
    {
    case 1:
        puts("There are not any file, so File for moving is \"copyText.txt\"");
        text = fopen("copyText.txt", "w");
        break;
    case 2:
        printf("Trying to open file %s\n", argv[1]);
        text = fopen(argv[1], "w");
        break;
    
    default:
        printf("There are too many files to open. Trying to open %s\n", argv[1]);
        text = fopen(argv[1], "w");
        break;
    }
    puts("Server searching connection");

    char Connect[32];
    memset(Connect, 1, 32);

	int transCode = mkfifo("transfer", 0600);

    int transferPipe = open("transfer", O_RDONLY);
    if (transferPipe != -1)
        puts("Successfully open transfer pipe");

    int succCode = 0; 
    while (succCode == 0)
        succCode = read(transferPipe, Connect, 32);
    
    if (succCode == -1)
        { puts("Problem to write!. I die"); return 1; }
    

    
    int LocalCode = mkfifo(Connect, 0600);
    int LocalPipe = open(Connect, O_RDONLY);
    if (LocalCode == -1 && LocalPipe == -1)
    {
        { puts("Problem to connect!. I die"); return 1; }
    }

    printf("Connection setup: %s\n", Connect);

    char exit = '!';
    unsigned char eof = 0;
    unsigned char buf[MAX];
    while(1 && exit != EXIT)
    {
        memset(buf, 1, MAX);
        int end = 0;
        end = read(LocalPipe, buf, MAX);
        unsigned char size = buf[0];
        fflush(NULL);
        fwrite(buf+1, 1, size, text);
        fflush(text);
        if (!size)
        {
            exit = EXIT;
            break;
        }
        if (!end)
        {
            puts("Connection lost");
            return 3;
        }
    }


    remove(Connect);

    puts("Copy ended");

    return 0;
}