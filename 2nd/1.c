#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>





int main(int argc, char * argv[])
{
    if (argc == 1)
        return 1;
   

    int pipefd[2];
    pid_t cpid;
    if (pipe(pipefd) == -1)
        return 2;
    
    cpid = fork();

    if (cpid == 0)//child
    {
        char buf[10];
        int i = 0;
        for (i = 0; i < 10; i++)
        {
            buf[i] = 0;
        }
    
        int size = read(pipefd[0], buf, 10);
        
        FILE * tmp = fopen(buf, "r");
        fread(buf, 1, 10 ,tmp);

        write(pipefd[1], buf, 10);

        close(pipefd[1]);
        close(pipefd[0]);

        fclose(tmp);

        return 0;
        /* its work!
        FILE * file = fopen(buf, "r");

        fread(buf, 1, 10, file);
        FILE * tmp = fopen("tmp", "w");
        fwrite(buf, 1, 10, tmp);

        fclose(file);
        fclose(tmp);

        return 0;      */
    }
    else // parent
    {
        write(pipefd[1], argv[1], strlen(argv[1]));
        close(pipefd[1]);
        wait(NULL);

        char buf[10];
        int i = 0;
        for (i = 0 ; i < 10; i++)
        {
            buf[i] = 0;
        }

        read(pipefd[0], buf, 10);

        printf("%s", buf);

        close(pipefd[1]);
        close(pipefd[0]);
        /*
        FILE * tmp = fopen("tmp", "r");
        char buf[10];
        int i = 0;
        for (i = 0; i < 10;  i++)
        {
            buf[i] = 0;
        }
        fread(buf, 1 ,10, tmp);
        printf("%s", buf);*/



    }
    

    return 0;
}