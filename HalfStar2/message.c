#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

struct msgbuf {
   long mType;
   int pid;
};

int main(int argc, char * argv[])
{
    if (argc == 1)
        return 0;

    char *endptr;
    int messKey = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (messKey == -1)
        return -1; 

    int n = strtol(argv[1], &endptr, 10);

    int i = 0;
    for (i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
            break;

        if (i == n - 1)
            return 0;
    }

    pid_t pid = getpid ();

    struct msgbuf msg;
    struct msgbuf rec;
    msg.mType = i + 1;
    msg.pid = pid;


    // if (i == 3) sleep(1);
    // if (i == 10) sleep(2);
    // if (i == 20) sleep(3);
    // if (i == 30) sleep(4);

    struct msqid_ds info; 
    info.msg_qnum = 0;

    while(info.msg_qnum != i)
    {
        if (msgctl(messKey, IPC_STAT, &info) != 0)
        {
           puts("Fail to get info");
           return -3;
        }
    }

    printf("%d\n", i);

    if (msgsnd(messKey, &msg, 4, 0) == -1)
    {
        puts("Fails to send message");
        return -2;
    }

    if (msgctl(messKey, IPC_STAT, &info) != 0)
    {
        puts("Fail to get info");
        return -3;
    }
    if (info.msg_qnum == n)
        msgctl(messKey, IPC_RMID, 0);

    return 0;
}
