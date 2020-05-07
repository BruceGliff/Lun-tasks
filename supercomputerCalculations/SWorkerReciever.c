#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "SWorkerReciever.h"

#define ERROR(a)    \
    {               \
        perror(a);  \
        abort();    \
    }


int rcvFromWorker(struct sockaddr_in * worker_addr);
void SendPortToWorker(int port, struct sockaddr_in * worker_addr);

void * reciever(void * data)
{ 
    struct sockaddr_in worker_addr;
    int const size_addr = sizeof(struct sockaddr_in);

    while(1)
    {
        rcvFromWorker(&worker_addr);    
        SendPortToWorker(100, &worker_addr);
    }

    return NULL;
}

int rcvFromWorker(struct sockaddr_in * worker_addr)
{
    puts("Rcv from worker begin");
    struct sockaddr_in serv_addr;
    int recaddrlen = sizeof(struct sockaddr_in);

    int port = 101;
    int bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcast_sock < 0)
        ERROR("Err create broadcast socket")
    int broadcastEnable=1;
    if(setsockopt(bcast_sock,SOL_SOCKET,SO_BROADCAST,&broadcastEnable,sizeof(int)) == -1)
        ERROR("Err set sockopt")

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;//BROADCAST; // ANY
    serv_addr.sin_port = htons(port);

    if(bind(bcast_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))==-1)
    {
            close(bcast_sock);
            ERROR("Err bind");
    }

    char recvString[100];
    memset(worker_addr, 0, recaddrlen);
    int res = recvfrom(bcast_sock, recvString, 99, 0, (struct sockaddr *) worker_addr, &recaddrlen);
    if (res < 0)
    {
        close(bcast_sock);
        ERROR("Err recvfrom");
    }

    recvString[res] = '\0';
    printf("Accept: %s\n", recvString);

    if(close(bcast_sock) == -1)
        ERROR("CLOSE err in rcv");

    puts("Rcv from serv end");
    
    return 0;
}


void SendPortToWorker(int port, struct sockaddr_in * worker_addr)
{
    puts("Sending begin...");

    int bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcast_sock < 0)
        ERROR("cannot create sock for broadcast\n");

    int broadcastEnable = 1;
    int ret = setsockopt(bcast_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret < 0)    
        ERROR("cannot set up socket for broadcast\n");


    struct sockaddr_in to_worker;
    to_worker.sin_family = AF_INET;
    memcpy(&to_worker.sin_addr, &worker_addr->sin_addr, sizeof(to_worker.sin_addr));
    to_worker.sin_port = htons(5);

    if(sendto(bcast_sock, &port, sizeof(int), 0, (struct sockaddr *)&to_worker, sizeof(struct sockaddr_in)) < 0)
        ERROR("sendto");

    if(close(bcast_sock))
        ERROR("close worker");

    puts("Sending end");

    return;
}