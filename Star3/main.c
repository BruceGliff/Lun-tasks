#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/prctl.h>

pid_t reader_pid;
pid_t writer_pid;

int bit;
char tmp;

int flag = 1;
int ChFlag = 1;

FILE * openFile(int argc, char * argv[]);
void voidHandler(int s){flag = 0;}
void Handler1(int s)
{
    tmp |= 1 << bit;

    if (bit == 7)
    {
	    printf("%c", tmp);
	    tmp = 0;
    }

    bit = (bit + 1) % 8;

    kill(reader_pid, SIGURG);
}
void Handler0(int s)
{
    tmp |= 0 << bit;

    if (bit == 7)
    {
	    printf("%c", tmp);
	    tmp = 0;
    }

    bit = (bit + 1) % 8;

    kill(reader_pid, SIGURG);
}

void ChildHandler(int sig)
{
	exit(1);
}
void ChildExit(int sig)
{
	exit(1);
}

int main(int argc, char **argv)
{
    int pipefd[2];
    pipe(pipefd);


    reader_pid = getpid();

    // prepare to child's death
    sigset_t ChildDeath;
    signal(SIGCHLD, &ChildHandler);
    sigemptyset(&ChildDeath);
    sigaddset(&ChildDeath, SIGCHLD); 
	
    // wait part
    sigset_t Wait;
    int sig;
    signal(SIGURG, &voidHandler);
    sigemptyset(&Wait);
    sigaddset(&Wait, SIGURG);
    
    // prepare for parent's death
    sigset_t ChExit;
    int SigExit;
    signal(SIGHUP, &ChildExit);
    sigemptyset(&ChExit);
    sigaddset(&ChExit, SIGHUP);


    pid_t fork_pid = fork();
    if (fork_pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (fork_pid == 0) {
        // writer
	prctl(PR_SET_PDEATHSIG, SIGHUP);
	writer_pid = getpid();

        // prepare part
        sigset_t Get1;
        int SigGet1;
        signal(SIGUSR2, &Handler1);
        sigemptyset(&Get1);
        sigaddset(&Get1, SIGUSR2);

        sigset_t Get0;
        int SigGet0;
        signal(SIGUSR1, &Handler0);
        sigemptyset(&Get0);
        sigaddset(&Get0, SIGUSR1);

        // sync part
        close(pipefd[0]);
        write(pipefd[1], "", 1); // process is ready
        close(pipefd[1]);

	// wait part
        sigwait(&Wait, &sig);

        exit(EXIT_SUCCESS);
    }
    if (fork_pid > 0) 
    {
        // reader

	writer_pid = fork_pid;
	FILE * file = openFile(argc, argv);
	if (!file)
		return -1;
	
	// wait part
	sigset_t Wait;
        int sig;
        signal(SIGURG, &voidHandler);
        sigemptyset(&Wait);
        sigaddset(&Wait, SIGURG);

	// sync part
        char c;
        close(pipefd[1]);
        read(pipefd[0], &c, 1); // process is wait
        close(pipefd[0]);
	// send part
	while(1)
	{
		tmp = fgetc(file);
		if (feof(file)) break;
		bit = 0;
		while(bit < 8)
		{
			flag = 1;

			if (tmp & (1<<bit))
				kill(writer_pid, SIGUSR2);
			else
				kill(writer_pid, SIGUSR1);
			bit++;		
			//sigwait(&Wait, &sig);

			//busy wait

			while(flag){}
		}
	}
        kill(writer_pid, SIGURG);
    }
}



FILE * openFile(int argc, char * argv[])
{
    FILE * text = NULL;
    switch (argc)
    {
    case 1:
        text = fopen("Text.txt", "r");
        break;
    case 2:
        text = fopen(argv[1], "r");
        break;
    
    default:
        text = fopen(argv[1], "r");
        break;
    }

    return text;
}
