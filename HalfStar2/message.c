#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

struct msgbuf {
   long mType;
   char trash;
};

int main(int argc, char * argv[])
{
    if (argc == 1)
        return 0;

    struct msgbuf msg;
    msg.trash = 0;

    char *endptr;
    int messKey = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (messKey == -1)
        return -1; 

    int n = strtol(argv[1], &endptr, 10);

    int i = 0;
    for (i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            goto here;
        }
        if (pid == 0)
            break;

        if (i == n - 1)
        {
            here:
            msg.mType = 1;
            if (msgsnd(messKey, &msg, 1, 0) == -1)
            {
                puts("Fails to send message");
                return -2;
            }

            int j = 0;
            for (j = 0; j <= i; j++)
                wait(NULL);

            msgctl(messKey, IPC_RMID, NULL);
            
            return 0;
        }
    }

    struct msgbuf rec;

    if (msgrcv(messKey, &rec, 1, i + 1, 0) == -1)
    {
        puts("Fails to recive message");
        return -2;
    }
    
    printf("%d ", i + 1);
    fflush(NULL);

    msg.mType = i + 2;
    if (msgsnd(messKey, &msg, 1, 0) == -1)
    {
        puts("Fails to send message");
        return -2;
    }
    return 0;
}
