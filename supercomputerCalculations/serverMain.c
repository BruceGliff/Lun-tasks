#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "SWorkerReciever.h"
#include "STaskSender.h"

#define ERROR(a)    \
    {               \
        perror(a);  \
        exit(-1);   \
    }


int establishConnection(int port);

int main()
{
    //create pipe for structures and pthread
    int const size_addr = sizeof(struct sockaddr_in);

    int pipefd[2];
    if (pipe(pipefd) == -1)
        ERROR("Err create pipe");

    pthread_t sync_th;
    if (pthread_create(&sync_th, NULL, reciever, NULL) != 0)
        ERROR("Err create thread");


    struct sockaddr_in worker_addr;
    int port = 102;

    struct pthread_t * th_queue[256];
    int free_indx = 0;


    int listen_sk = establishConnection(100);
    listen(listen_sk, 256);

    while(1)
    { 
        BEGIN:
        memset(&worker_addr, 0, size_addr);
        int a=0;
        puts("ACCEPT:");
        int newSocket = accept(listen_sk, (struct sockaddr *) &worker_addr, &a);
        printf("%d\n", newSocket);
        goto BEGIN;
        if (newSocket == -1)
            ERROR("Err accept");
        th_queue[free_indx] = (struct pthread_t *) malloc (sizeof(pthread_t));
        //if (pthread_create(th_queue[free_indx++], NULL, reciever,) != 0)
            //ERROR("Err create thread");
    }

    close(pipefd[0]);
    pthread_join(sync_th, NULL);

    return 0;
}



int establishConnection(int port)
{
    int sk = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sk, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        ERROR("ERROR on binding");

    return sk;    
}