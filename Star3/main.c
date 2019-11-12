#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define CHARFULL SIGURG
#define ANSWER SIGILL

FILE * GetFile(int argc, char * argv[]);


void voidD(int s)
{
}


// Global part
int reader_pid; // child
int writer_pid; // parent

char c;
int b;

// HANDLERS
void recieve1(int s)
{
//puts("s");
	c |= 1 << b;
	b = (b + 1) % 8;
	if (b)
	{
		kill(reader_pid, ANSWER);
//puts("R");
	}
	else	
		kill(writer_pid, CHARFULL);
}
void recieve0(int s)
{
//puts("s");
	c |= 0 << b;
	b = (b + 1) % 8;
	if (b)
	{
		kill(reader_pid, ANSWER);
//puts("R");
	}
	else	
		kill(writer_pid, CHARFULL);
}

void exitH(int sig)
{
	exit(2);
}



int main(int argc, char * argv[])
{
	writer_pid = getpid();
	
	sigset_t WaitSignals;
	sigfillset(&WaitSignals);
	sigdelset(&WaitSignals, SIGUSR1);
	sigdelset(&WaitSignals, SIGUSR2);
	sigset_t _WaitSignals;
	sigemptyset(&_WaitSignals);
	sigaddset(&_WaitSignals, SIGUSR1);
	sigaddset(&_WaitSignals, SIGUSR2);	
	sigprocmask(SIG_BLOCK, &_WaitSignals, NULL);

	// SIGCHILD
	sigset_t ReadOverSet;
	sigemptyset(&ReadOverSet);
	sigaddset(&ReadOverSet, SIGCHLD);
	struct sigaction Exit;
	Exit.sa_handler = exitH;
	sigaction(SIGCHLD, &Exit, NULL);
		
	struct sigaction Rec0, Rec1;
	Rec0.sa_handler = recieve0;
	Rec1.sa_handler = recieve1;
	sigaction(SIGUSR1, &Rec0, NULL);
	sigaction(SIGUSR2, &Rec1, NULL);

	int fork_pid = fork();
	if(fork_pid < 0)
		exit(EXIT_FAILURE);

	if (fork_pid)
	{
		// parent
		reader_pid = fork_pid;
		printf("par %d\n", writer_pid);
	       	fflush(NULL);	


		// preapre part
		sigset_t CharFull;
		sigfillset(&CharFull);
		sigdelset(&CharFull, CHARFULL);
	//	sigdelset(&CharFull, SIGUSR1);
	//	sigdelset(&CharFull, SIGUSR2);
		sigset_t _CharFull;
		sigemptyset(&_CharFull);
		sigaddset(&_CharFull, CHARFULL);



//puts("Asd");
		// wait
		sigset_t any; sigemptyset(&any);
		sigprocmask(SIG_UNBLOCK, &_WaitSignals, NULL);
		sigsuspend(&any);
//puts("Asd");
		while(1)
		{
			sigsuspend(&CharFull);
			printf("%c", c);
			fflush(NULL);
			c = 0;
			sigprocmask(SIG_BLOCK, &_CharFull, NULL);
			kill(reader_pid, ANSWER);
		}


		sigsuspend(&ReadOverSet);
	}
	else
	{
		// child
		reader_pid = getpid();
		printf("child %d\n", reader_pid);
	       	fflush(NULL);

		// prepare part
		sigset_t Ans;
		sigfillset(&Ans);
		sigdelset(&Ans, ANSWER);
		sigdelset(&Ans, SIGINT);
		sigset_t _Ans;
		sigemptyset(&_Ans);
		sigaddset(&_Ans, ANSWER);
		struct sigaction sa;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sa.sa_handler = voidD;
		sigaction(ANSWER, &sa, NULL);
	       	
		sigprocmask(SIG_SETMASK, &_Ans, NULL);

		FILE * f = GetFile(argc, argv);
		if (!f)
			return -1;

		char tmp = fgetc(f);

		while(!feof(f))
		{
			int bit = 0;
			while(bit < 8)
			{

				if (tmp & (1 << bit))
				{
					bit++;
					kill(writer_pid, SIGUSR2);
				}
				else
				{
					bit++;
					kill(writer_pid, SIGUSR1);
				}
//puts("A");
				sigsuspend(&Ans);
//puts("V");
			}
			tmp = fgetc(f);
		}
puts("ASD");	
		exit(1); // auto send SIGCHILD also READOVER
	}



	return 0;
}


FILE * GetFile(int argc, char * argv[])
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


