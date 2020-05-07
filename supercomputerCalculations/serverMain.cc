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
#include "Task.h"

#define ERROR(a)    \
    {               \
        perror(a);  \
        exit(-1);   \
    }


int establishConnection(int port);

int main()
{
    //create pipe for structures and pthread
    int const size_addr = sizeof(sockaddr_in);

    pthread_t sync_th;
    if (pthread_create(&sync_th, NULL, reciever, NULL) != 0)
        ERROR("Err create thread");


    sockaddr_in worker_addr;
    int port = 102;

    pthread_t * th_queue[256];
    int free_indx = 0;

    TasksQueue q;

    int listen_sk = establishConnection(100);
    listen(listen_sk, 256);

    while(1)
    { 
        memset(&worker_addr, 0, size_addr);
        socklen_t a=0;
        puts("ACCEPT:");
        int newSocket = accept(listen_sk, (struct sockaddr *) &worker_addr, &a);
        printf("%d\n", newSocket);
        if (newSocket == -1)
            ERROR("Err accept");
        th_queue[free_indx] = (pthread_t *) malloc (sizeof(pthread_t));
        ConnectionSettings set = {&q, newSocket};
        if (pthread_create(th_queue[free_indx++], NULL, TaskSender, &set) != 0)
            ERROR("Err create thread");
    }

    pthread_join(sync_th, NULL);

    return 0;
}



int establishConnection(int port)
{
    int sk = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sk, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        ERROR("ERROR on binding");

    return sk;    
}