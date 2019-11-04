#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

FILE * openFile(int argc, char * argv[]);


void Recieve0(int sig)
{
	puts("Get0");
    fflush(NULL);
}

void Recieve1(int sig)
{
    puts("Get1");
    fflush(NULL);
}


void reader(int argc, char * argv[], pid_t writerPid)
{
    sigset_t SigWriterReady;
    sigemptyset(&SigWriterReady);                                                             
    sigaddset(&SigWriterReady, SIGURG); 
    int sig1;                                                                      
    sigwait(&SigWriterReady, &sig1);

    FILE * file = openFile(argc, argv);
    if (file == NULL)
    {
        puts("Cannot open file.");
        return;
    }


    int killStatus = kill(writerPid, SIGUSR1);
    sleep(1);
    kill(writerPid, SIGUSR2);
    puts("ASd");
}


void writer(pid_t ReaderPid)
{
    // begin preparation part

    struct sigaction read0;
    memset(&read0, 0, sizeof(read0));
    read0.sa_handler = Recieve0;
    sigset_t SigRead0;
    sigemptyset(&SigRead0);                                                             
    sigaddset(&SigRead0, SIGUSR1); 
    read0.sa_mask = SigRead0;
    sigaction(SIGUSR1, &read0, 0);

    struct sigaction read1;
    memset(&read1, 0, sizeof(read1));
    read1.sa_handler = Recieve1;
    sigset_t SigRead1;
    sigemptyset(&SigRead1);                                                             
    sigaddset(&SigRead1, SIGUSR2); 
    read1.sa_mask = SigRead1;
    sigaction(SIGUSR1, &read1, 0);
    puts("end");
    // end preparation part
    kill(ReaderPid, SIGURG);

    sleep(100);
}





int main(int argc, char * argv[]) 
{
    pid_t writerPid = getpid();
    pid_t forkPid = fork();

    if (forkPid == -1)
    {
        puts("Wrong creation child.");
        fflush(NULL);
        return -1;
    }

    if (!forkPid)
        reader(argc, argv, writerPid);
    else
        writer(forkPid);
    


    return 0;

}



FILE * openFile(int argc, char * argv[])
{
    FILE * text = NULL;
    switch (argc)
    {
    case 1:
        puts("There are not any file, so File for coping is \"Text.txt\"");
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

    return text;
}