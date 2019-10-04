#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define EXIT 'c'

#define MAX 1024*1024


void Hash(int ID, char * connect);
int len(const char * str);

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

    if (text == 0)
    {
        puts("Cannot open file");
        return 1;
    }
    puts("Server searching connection");



    int ID = getpid();
    char Connect[32];
    memset(Connect, 1, 32);
    Hash(ID, Connect);
    remove(Connect);

    int LocalPipe = mkfifo(Connect, 0600);
    int local_fd = 999;
    local_fd = open(Connect, O_RDONLY | O_NONBLOCK);

	int transPipe = mkfifo("transfer", 0600);
    int trans_fd = open("transfer", O_WRONLY);
    int isSucc = write(trans_fd, Connect, 32);

    puts("Successfully open transfer pipe");
    printf("Connection setup: %s\n", Connect);
                    
    unsigned char eof = 0;
    unsigned char buf[MAX];

    int TriesToRead = 1;
    int isConnected = 0;
    while(1)
    {
        if (TriesToRead > 5)
        {
            puts("Fail to read! Times out");
            puts("Connection lost");
            remove(Connect);
            return 3;

        }
        memset(buf, 0, MAX);
        int end = 0;
        end = read(local_fd, buf, MAX);

        if(end == 0 && !isConnected)
        {
            printf("%d try to read from local pipe of 5\n", TriesToRead);
            TriesToRead++;
            sleep(1);
            continue;
        }

        fcntl(local_fd, F_SETFL, O_RDONLY);
        isConnected = 1;
        if (end == 0)
            break;
        fwrite(buf, 1, end, text);

        fflush(text);
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