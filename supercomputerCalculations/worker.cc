#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h> 

#include "Thread.h"

#define ERROR(a)    \
    {               \
        perror(a);  \
        exit(-1);   \
    }

void * broadcastToServer(void *);
struct sockaddr rcvFromServer();
int establishConnection(int port, struct sockaddr_in * server);
int rcvPortFromServer(struct sockaddr_in * serv_addr);
int work_with_task(int sk);

int broadcast_port = 0;

int main(int argc, char ** argv)
{
	if (argc == 1)
		broadcast_port = 5;
	else
		broadcast_port = strtol(argv[1], NULL, 10);

	pthread_t broadcast_th;
	if (pthread_create (&broadcast_th, NULL, broadcastToServer, NULL) == -1)
		ERROR("Err create broadcast thread");
	 
//    broadcastToServer();
    struct sockaddr_in server;
    int port = rcvPortFromServer(&server);
    pthread_cancel(broadcast_th);
    int sk = establishConnection(port, &server);
    work_with_task(sk);


    
}


int work_with_task(int sk)
{
    puts("Begin work with task");
    int th = GetCpuConfiguration();
    fd_set readfd;
    struct timeval to;
    while(1)
    {
        to.tv_sec = 20;
        to.tv_usec = 0;
        
        FD_ZERO(&readfd);
        FD_SET(sk, &readfd);
        int r = select(sk + 1, &readfd, NULL, NULL, &to);

        if (r == -1)
        {
            perror("Connection lost select");
            if (shutdown(sk, SHUT_RDWR) == -1)
                ERROR("Err shutdown sk");
            return 0;
        }
        if (r == 0)
        {
            perror("Timeout lost select");
            if (shutdown(sk, SHUT_RDWR) == -1)
                ERROR("Err shutdown sk");
            return 0;
        }
        Task t;
        int res = read(sk, &t, sizeof(Task));
        if (res != sizeof(Task))
            ERROR("End work with task")
        printf("%f %f\n", t.begin, t.end);
        double ans = 5;
        
        Task_local * TASK = Task_create(CPU_info, &t);
        
        Threads * THREAD =  Threads_create(TASK);
        ans = launch(THREAD, TASK);
        printf("%e\n", ans);
        
        Threads_delete(THREAD);
    	Task_delete(TASK);
        
        res = write(sk, &ans, sizeof(double));
        if (res != sizeof(double))
            ERROR("Err write task")
    }

}


int establishConnection(int port, struct sockaddr_in * server)
{
    puts("Begin establish connection");
    int sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk < 0) 
        perror("ERROR opening socket");

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
    memcpy(&serverAddr.sin_addr, &server->sin_addr, sizeof(serverAddr.sin_addr));
    serverAddr.sin_port = htons(port);
    
    if (connect(sk, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) 
        perror("ERROR connecting");

    puts("End establish connection");

    return sk;
}

void * broadcastToServer(void *)
{
    puts("Broadast begin");

    struct sockaddr_in ToServer;
    int port = 101;
    int bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcast_sock < 0)
        perror("cannot create sock for broadcast\n");

    int broadcastEnable = 1;
    int ret = setsockopt(bcast_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret < 0)    
        perror("cannot set up socket for broadcast\n");


    ToServer.sin_family = AF_INET;
    ToServer.sin_addr.s_addr = INADDR_BROADCAST; // ANY
    ToServer.sin_port = htons(port);

int tries = 0;
while(++tries != 11) {
	printf("Try [%d - 10]\n", tries);
    if(sendto(bcast_sock, &broadcast_port, sizeof(int), 0, (struct sockaddr *)&ToServer, sizeof(struct sockaddr_in)) < 0)
        perror("sendto");
    sleep(1);
}

    if(close(bcast_sock))
        perror("close worker");


    puts("Cannot establish connection");
    exit(-1);

    return NULL;
}

int rcvPortFromServer(struct sockaddr_in * serv_addr)
{
    puts("Rcv from server begin");
    struct sockaddr_in my_addr;
    int recaddrlen = sizeof(struct sockaddr_in);

    int port = broadcast_port;
    int bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcast_sock < 0)
        ERROR("Err create broadcast socket");
    int broadcastEnable=1;
    if(setsockopt(bcast_sock,SOL_SOCKET,SO_BROADCAST,&broadcastEnable,sizeof(int)) == -1)
        ERROR("Err set sockopt")

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(port);

    if(bind(bcast_sock, (struct sockaddr *) &my_addr, sizeof(my_addr))==-1)
    {
            close(bcast_sock);
            ERROR("Err bind, pls set new port (default 5)");
    }

    int serv_port;

    socklen_t l = sizeof(sockaddr_in);
    int res = recvfrom(bcast_sock, &serv_port, 4, 0, (struct sockaddr *)serv_addr, &l);
    if (res < 0)
    {
        close(bcast_sock);
        ERROR("Err recvfrom");
    }

    printf("Accept port: %d\n", serv_port);

    if(close(bcast_sock) == -1)
        ERROR("CLOSE err in rcv");

    puts("Rcv from serv end");
    
    return serv_port;
}
