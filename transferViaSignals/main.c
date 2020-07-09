#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/prctl.h>


#define CHARFULL    SIGURG
#define ANSWER      SIGHUP
#define BIT0        SIGUSR1
#define BIT1        SIGUSR2
#define QUIT        SIGCHLD

FILE * GetFile(int argc, char * argv[]);


// Global part
int reader_pid; // child
int writer_pid; // parent

char c;
int bit;
int upd_f;

// HANDLERS
void void0(int s){}
void Exit(int s) { exit(1); }
void Recieve1(int s)
{
	c |= 1 << bit;
	bit++;
		kill(reader_pid, ANSWER);
}
void Recieve0(int s)
{
	c |= 0 << bit;
	bit++;
		kill(reader_pid, ANSWER);
}
void Update(int s)
{ upd_f = 1; }


int main(int argc, char * argv[])
{
	writer_pid = getpid();

	sigset_t RecBit;
	sigemptyset(&RecBit);
	sigaddset(&RecBit, BIT0);
	sigaddset(&RecBit, BIT1);
	sigaddset(&RecBit, CHARFULL);
	sigset_t _RecBit;
	sigprocmask(SIG_BLOCK, &RecBit, &_RecBit);
	
	struct sigaction Rec0_sa, Rec1_sa, upd_sa;
	Rec0_sa.sa_handler = Recieve0;
	Rec0_sa.sa_flags = SA_RESTART;
	Rec1_sa.sa_flags = SA_RESTART;
	Rec1_sa.sa_handler = Recieve1;
	sigaction(BIT0, &Rec0_sa, NULL);
	sigaction(BIT1, &Rec1_sa, NULL);
	
	struct sigaction Exit_sa; 
	Exit_sa.sa_handler = Exit;
	sigaction(QUIT, &Exit_sa, NULL);

    	int fork_pid = fork();
	if(fork_pid < 0)
		exit(EXIT_FAILURE);

    	if (fork_pid)
	{
		// parent WRITER
        	reader_pid = fork_pid;
		fflush(NULL);
		upd_sa.sa_handler = Update;
		upd_sa.sa_flags = SA_RESTART;
		sigaction(CHARFULL, &upd_sa, NULL);

		c = 0;
		bit = 0;
		upd_f = 0;


		while(1)
		{
			sigsuspend(&_RecBit);
			if(upd_f)
			{	
				upd_f = 0;
				printf("%c", c);
				fflush(NULL);
				c = 0;
				bit = 0;
				kill(reader_pid, ANSWER);
			}
		}
        
    	}
	else
	{
		// child READER
		prctl(PR_SET_PDEATHSIG, QUIT);	
		reader_pid = getpid();
		fflush(NULL);

		sigset_t Ans;
		sigemptyset(&Ans);
		sigaddset(&Ans, ANSWER);
		struct sigaction ans_sa; ans_sa.sa_handler = void0;
		ans_sa.sa_flags = SA_RESTART;
		sigaction(ANSWER, &ans_sa, NULL);
		sigset_t _Ans;
		sigprocmask(SIG_BLOCK, &Ans, &_Ans);

       		FILE * f = GetFile(argc, argv);
		if (!f)
		{
			puts("No file");
			return -1;
		}

		c = fgetc(f);

		while(!feof(f))
		{
			bit = 0;
			while(bit < 8)
			{
				if (c & (1 << bit))
				{
					bit++;
					kill(writer_pid, BIT1);
				}
				else
				{
					bit++;
					kill(writer_pid, BIT0);
				}
				sigsuspend(&_Ans);
			}
               		kill(writer_pid, CHARFULL);
			sigsuspend(&_Ans);
			c = fgetc(f);
		}
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
