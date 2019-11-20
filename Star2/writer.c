#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>

#define MAX 100
enum
{
	SEM_WONLY,
	SEM_RONLY,
	SEM_READ,
	SEM_WRITE
};

struct data
{
	char data[MAX];
	int size;
};

int main()
{
	key_t key = ftok("/tmp", 'b');

	struct data * d;
	// create sems
	// if exists then connect
	int sem_id = semget(key, 4, IPC_CREAT | IPC_EXCL | 0666);
	if (sem_id == -1)
		sem_id = semget(key, 4, 0666);	

	struct sembuf sb[3];
	sb[0].sem_num = SEM_WONLY;
	sb[0].sem_op = 0;
	sb[0].sem_flg = 0;
	sb[1].sem_num = SEM_WONLY;
	sb[1].sem_op = 1;
	sb[1].sem_flg = SEM_UNDO;


	//struct sembuf tmp[4];
	//sb[0].sem_num = SEM_WONLY;
	//sb[0].sem_op = 0;
	//sb[0].sem_flg = 0;
	//sb[1].sem_num = SEM_WONLY;
	//sb[1].sem_op = 1;
	//sb[1].sem_flg = SEM_UNDO;


//printf("%d", sem_id);fflush(NULL);
	semop(sem_id, sb, 2);

	sb[0].sem_num = SEM_RONLY;
	sb[0].sem_op = -1;
	sb[0].sem_flg = 0;
	sb[1].sem_num = SEM_RONLY;
	sb[1].sem_op = 1;
	sb[1].sem_flg = 0;
	semop(sem_id, sb, 2);
//puts("A");
	int shm_id = shmget(key, sizeof(struct data), IPC_CREAT | IPC_EXCL | 0666);
	if (shm_id == -1)
		shm_id = shmget(key, sizeof(struct data), 0666);

	d = shmat(shm_id, NULL, 0);

	struct sembuf sb1[3];
	sb1[0].sem_num = SEM_RONLY;
	sb1[0].sem_op = -1;
	sb1[0].sem_flg = IPC_NOWAIT;

	sb1[1].sem_num = SEM_RONLY;
	sb1[1].sem_op = 1;
	sb1[1].sem_flg = IPC_NOWAIT;
	
	sb1[2].sem_num = SEM_READ;
	sb1[2].sem_op = -1;
	sb1[2].sem_flg = 0;
	
	sb[0].sem_num = SEM_WRITE;
	sb[0].sem_op = 1;
	sb[0].sem_flg = SEM_UNDO;

	struct sembuf balance[2];
	balance[0].sem_num = SEM_WRITE;
	balance[0].sem_op = 1;
	balance[0].sem_flg = 0;
	balance[1].sem_num = SEM_WRITE;
	balance[1].sem_op = -1;
	balance[1].sem_flg = SEM_UNDO;

	semop(sem_id, balance, 2);
	while(semop(sem_id, sb1, 3) != -1)
	{
//puts("A");
		write(1, d->data, d->size);
//printf("\n%d\n", d->size);fflush(NULL);

		semop(sem_id, sb, 1);
		semop(sem_id, balance, 2);
//puts("B");
	}
	shmdt(d);
			
//puts("END");	
}

