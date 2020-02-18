#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define MAX 100
int GetFile(int argc, char * argv[]);

enum
{
	SEM_WONLY,
	SEM_RONLY,
	SEM_READ,
	SEM_WRITE,
	SEM_WWORK,
	SEM_RWORK
};

struct data
{
	char data[MAX];
	int size;
};

int main(int argc, char * argv[])
{
	key_t key = ftok("/tmp", 'b');

	struct data * d;

	int sem_id = semget(key, 6, IPC_CREAT | IPC_EXCL | 0666);
	if (sem_id == -1)
		sem_id = semget(key, 6, 0666);

	struct sembuf sb[3];
	sb[0].sem_num = SEM_RONLY;
	sb[0].sem_op = 0;
	sb[0].sem_flg = 0;
	sb[1].sem_num = SEM_WWORK;
	sb[1].sem_op = 0;
	sb[1].sem_flg = 0;
	sb[2].sem_num = SEM_RONLY;
	sb[2].sem_op = 1;
	sb[2].sem_flg = SEM_UNDO;

	semop(sem_id, sb, 3);//OP/////////
	
	int shm_id = shmget(key, sizeof(struct data), IPC_CREAT | IPC_EXCL | 0666);
	if (shm_id == -1)
		shm_id = shmget(key, sizeof(struct data), 0666);

	d = shmat(shm_id, NULL, 0);
	memset(d, 0, sizeof(struct data));

	sb[0].sem_num = SEM_WONLY;
	sb[0].sem_op = -1;
	sb[0].sem_flg = 0;
	sb[1].sem_num = SEM_WONLY;
	sb[1].sem_op = 1;
	sb[1].sem_flg = 0;
	sb[2].sem_num = SEM_RWORK; //
	sb[2].sem_op = 1;
	sb[2].sem_flg = SEM_UNDO;
	semop(sem_id, sb, 3);//OP/////////////

//puts("b");	
	int fd = GetFile(argc, argv);
	if (fd < 0)
		exit(1);

	struct sembuf sb1[4];
	sb1[0].sem_num = SEM_WONLY;
	sb1[0].sem_op = -1;
	sb1[0].sem_flg = IPC_NOWAIT;

	sb1[1].sem_num = SEM_WONLY;
	sb1[1].sem_op = 1;
	sb1[1].sem_flg = IPC_NOWAIT;

	sb[0].sem_num = SEM_READ;
	sb[0].sem_op = 1;
	sb[0].sem_flg = 0;//SEM_UNDO;
	
	sb1[2].sem_num = SEM_WRITE;
	sb1[2].sem_op = -1;
	sb1[2].sem_flg = 0;//SEM_UNDO;

	struct sembuf balance[4];
	balance[0].sem_num = SEM_WRITE;
	balance[0].sem_op = 1;
	balance[0].sem_flg = SEM_UNDO;
	balance[1].sem_num = SEM_WRITE;
	balance[1].sem_op = -1;
	balance[1].sem_flg = 0;
	balance[2].sem_num = SEM_READ;
	balance[2].sem_op = 1;
	balance[2].sem_flg = SEM_UNDO;
	balance[3].sem_num = SEM_READ;
	balance[3].sem_op = -1;
	balance[3].sem_flg = 0;

	semop(sem_id, balance, 4);

	d->size = read(fd, d->data, MAX);

	do
	{
		semop(sem_id, sb, 1);//OP//////////////
		if (semop(sem_id, sb1, 3) == -1)//OP////////////
			break;			

		d->size = read(fd, d->data, MAX);
	} while (d->size != 0);

	shmdt(d);	
}


int GetFile(int argc, char * argv[])
{
	int text = 0;
	switch (argc)
    	{
    	case 1:
       		text = open("Text.txt", O_RDONLY);
        	break;
    	case 2:
        	text = open(argv[1], O_RDONLY);
        	break;
    
    	default:
        	text = open(argv[1], O_RDONLY);
        	break;
    	}

    return text;
}
	
