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

    if (text == NULL)
    {
        puts("File to move does not exist. I'm die");
        return 2;
    }
    puts("Client searching connection");
    
    char Connect[32];
    memset(Connect, 1, 32);

	int transCode = mkfifo("transfer", 0600);
    int trans_fd = open("transfer", O_RDONLY);

    puts("Successfully open transfer pipe");

    int isSucc = read(trans_fd, Connect, 32);
    if (!isSucc) { puts("No file to tranfer!"); return 0; };

    unsigned char eof = 0;
    unsigned char buf[MAX];

    puts("Creating local connection");
    
    int local_fd = 999;
    local_fd = open(Connect, O_WRONLY | O_NONBLOCK);
    if (local_fd == -1)
        { printf("Cannot connect to %s", Connect); return 5; }

    fcntl(local_fd, F_SETFL, O_WRONLY);

    printf("Connection setup: %s\n", Connect);

    int end = 0;
    while(!feof(text))        
    {
        memset(buf, 0, MAX);
        end = fread(buf, 1, MAX, text);
        write(local_fd, buf, end);
    }

    puts("Copy ended");

    return 0;
}