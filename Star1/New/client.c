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

void Hash(int ID, char * connect);

int main(int argc, char * argv[])
{   
    FILE * text = NULL;
    switch (argc)
    {
    case 1:
        puts("There are not any file, so File for moving is \"Text.txt\"");
        text = fopen("Text.txt", "r");
        break;
    case 2:
        printf("Trying to open file %s\n", argv[1]);
        text = fopen(argv[1], "r");
        break;
    
    default:
        printf("There are too many files to open. Trying to open %s\n", argv[1]);
        text = fopen(argv[1], "r");
        break;
    }

    if (text == NULL)
    {
        puts("File to move does not exist. I'm die");
        return 2;
    }
    puts("Client searching connection");
    
    int ID = getpid();
    char Connect[32];
    memset(Connect, 1, 32);
    Hash(ID, Connect);
    remove(Connect);

	int transCode = mkfifo("transfer", 0600);

    int transferPipe = open("transfer", O_WRONLY);
    if (transferPipe != -1)
        puts("Successfully open transfer pipe");

    int succCode = write(transferPipe, Connect, 32);

    if (succCode == -1)
        { puts("Problem to write!. I die"); fflush(NULL); return 1; }

    int LocalCode = 999;
    int LocalPipe = 999;

    puts("Creating local connection");

lReconnect:
    LocalCode = mkfifo(Connect, 0600);
    LocalPipe = open(Connect, O_WRONLY);
    if (LocalCode == -1 && LocalPipe == -1)
        { puts("Going try again. Removing connection..."); remove(Connect); goto lReconnect; }

    printf("Connection setup: %s\n", Connect);

    char exit = '!';
    unsigned char eof = 0;
    unsigned char buf[MAX];

    while (exit != EXIT && !feof(text))
    {   
        memset(buf, 0, MAX);
        // TODO make thread for checking exit status
        int end = 0;
        while(!feof(text))        
        {
            end = fread(buf + 1, 1, MAX - 1, text);
            buf[0] = end;
            write(LocalPipe, buf, MAX);
        }
        write(LocalPipe, &eof, 1);
    }
    
    if (exit == EXIT)
        { puts("EXIT"); return 0; }

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

