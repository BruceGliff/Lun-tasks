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
FILE * f;

int flag = 1;
int ChFlag = 1;

FILE * openFile(int argc, char * argv[]);
void voidHandler(int s)
{
	if (feof(f))
		exit(1);

	if (bit == 0)
	{
		// send signal to update tmp
		// wait answer
		// tmp = newChar
		tmp = fgetc(f);

	}

	if (tmp & (1 << bit))
	{
		bit = (bit + 1) % 8;
		kill(writer_pid, SIGUSR2);
	}
	else
	{
		bit = (bit + 1) % 8;
		kill(writer_pid, SIGUSR1);
	}
}

void Handler1(int s)
{
    tmp |= 1 << bit;

    if (bit == 7)
    {
	    // send signal to update tmp
	    // wait answer
	    // tmp = 0
	    printf("%c", tmp);
	    fflush(NULL);
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
	    // The same shit
	    printf("%c", tmp);
	    fflush(NULL);
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
    sigset_t ChildDeathSet;
    int SigChildDeath;
    sigemptyset(&ChildDeathSet);
    sigaddset(&ChildDeathSet, SIGCHLD); 
    struct sigaction ChildDeath;
    ChildDeath.sa_handler = ChildHandler;
    ChildDeath.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &ChildDeath, NULL);
	
    // wait part
    sigset_t Wait;
    int sig;
    sigemptyset(&Wait);
    sigaddset(&Wait, SIGURG);
    struct sigaction WaitSig;
    WaitSig.sa_handler = voidHandler;
    WaitSig.sa_flags = SA_RESTART;
    sigaction(SIGURG, &WaitSig, NULL);

    // prepare for parent's death
    struct sigaction PDeath;
    PDeath.sa_handler = ChildExit;
    PDeath.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &PDeath, NULL);

    pid_t fork_pid = fork();
    if (fork_pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (fork_pid == 0) {
        // writer
	prctl(PR_SET_PDEATHSIG, SIGHUP);
	writer_pid = getpid();
        // prepare part
	
	struct sigaction USR2;
        USR2.sa_handler = Handler1;
	USR2.sa_flags = SA_RESTART;
    	sigaction(SIGUSR2, &USR2, NULL);

	struct sigaction USR1;
        USR1.sa_handler = Handler0;
	USR1.sa_flags = SA_RESTART;
    	sigaction(SIGUSR1, &USR1, NULL);

        // sync part
        close(pipefd[0]);
        write(pipefd[1], "", 1); // process is ready
        close(pipefd[1]);
		

	// while(1)
	// 	wait signal to update 
	// 	dump tmp
	// 	send signal to continue

	// wait part
        sigwait(&Wait, &sig);

        exit(EXIT_SUCCESS);
    }
    if (fork_pid > 0) 
    {
        // reader

	writer_pid = fork_pid;
	f = openFile(argc, argv);
	if (!f)
		return -1;
	
	// sync part
        char c;
        close(pipefd[1]);
        read(pipefd[0], &c, 1); // process is wait
        close(pipefd[0]);


	// New send part
	
	// while(1)
	// if flag = 0
	// update tmp
	// wait signal to update

	voidHandler(0);

	sigwait(&ChildDeathSet, &SigChildDeath);
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
