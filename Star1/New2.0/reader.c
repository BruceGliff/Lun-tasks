#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define EXIT 'c'

#define MAX 256


void Hash(int ID, char * connect);
void * ConsolStatus(void * smth);

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


    int ID = getpid();
    char Connect[32];
    memset(Connect, 1, 32);
    Hash(ID, Connect);
    remove(Connect);

    int LocalPipe = mkfifo(Connect, 0600);
    int local_fd = 999;
    local_fd = open(Connect, O_RDONLY | O_NONBLOCK); // CHECK

    if (local_fd == -1 && LocalPipe == -1)
        { puts("Problem to connect!. I die"); remove(Connect); return 1; }

	int transPipe = mkfifo("transfer", 0600);
    int trans_fd = open("transfer", O_WRONLY);
    int isSucc = write(trans_fd, Connect, 32);

    puts("Successfully open transfer pipe");
    printf("Connection setup: %s\n", Connect);
                    
    unsigned char eof = 0;
    unsigned char buf[MAX + 1];

    int TriesToRead = 1;
    while(1)
    {
        if (TriesToRead > 5)
        {
            puts("Fail to read! Times out");
            puts("Connection lost");
            sleep(10);
            remove(Connect);
            return 3;

        }
        memset(buf, 1, MAX + 1);
        int end = 0;
        end = read(local_fd, buf, MAX + 1);
        unsigned char size = buf[0];

        if(end == 0)
        {
            printf("%d try to read from local pipe of 5\n", TriesToRead);
            TriesToRead++;
            //sleep(1);
            continue;
        }

        fcntl(local_fd, F_SETFL, O_RDONLY);

        fwrite(buf+1, 1, size, text);
        fflush(text);

        if (buf[MAX] == 127)
        {
            break;
        }
    }

    remove(Connect);

    puts("Copy ended");

    return 0;
}

void Hash(int ID, char * connect)
{
    int len = 0;
    while (ID > 0)
    {
        connect[len] = ID % 26 + 'a';
        ID /= 26;
        len++;
    }
    connect[len] = 0;

    strcat(connect, ".fifo");
}