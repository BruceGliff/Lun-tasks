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

#define ERROR(a)    \
    {               \
        perror(a);  \
        exit(-1);   \
    }

void broadcastToServer();
struct sockaddr rcvFromServer();
void establishConnection(int port);
int rcvPortFromServer();

int main()
{
    broadcastToServer();
    int port = rcvPortFromServer();
    establishConnection(port);


    puts("before sleep");
    sleep(1);
    puts("after sleep");

    
    
}
void establishConnection(int port)
{
    int sk = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr, cli_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sk, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        ERROR("ERROR on binding");

    listen(sk, 5);

    int clilen = sizeof(cli_addr);
    int newsockfd = accept(sk, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        ERROR("ERROR on accept");

    int i = 0;
    while (++i != 4)
    {
        char task[5];
        task[4] = 0;
        read(newsockfd, task, 4);
        printf("%s\n", task);
        write(newsockfd, "ANS", 3);
    }

    shutdown(newsockfd, SHUT_RDWR);

    puts("finish establ connectio");
}

void broadcastToServer()
{
    puts("BroadCastBegin");


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

    char const * mess = "TOSERVER";

    if(sendto(bcast_sock, mess, strlen(mess), 0, (struct sockaddr *)&ToServer, sizeof(struct sockaddr_in)) < 0)
        perror("sendto");

    if(close(bcast_sock))
        perror("close worker");

    puts("BroadcastEnd");

    return;
}

int rcvPortFromServer()
{
    puts("Rcv from server begin");
    struct sockaddr_in serv_addr;
    int recaddrlen = sizeof(struct sockaddr_in);

    int port = 5;
    int bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcast_sock < 0)
        ERROR("Err create broadcast socket");
    int broadcastEnable=1;
    if(setsockopt(bcast_sock,SOL_SOCKET,SO_BROADCAST,&broadcastEnable,sizeof(int)) == -1)
        ERROR("Err set sockopt")

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if(bind(bcast_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))==-1)
    {
            close(bcast_sock);
            ERROR("Err bind");
    }

    int serv_port;
    int res = recvfrom(bcast_sock, &serv_port, 4, 0, NULL, NULL);
    if (res < 0)
    {
        close(bcast_sock);
        ERROR("Err recvfrom");
    }

    printf("Accept: %d\n", serv_port);

    if(close(bcast_sock) == -1)
        ERROR("CLOSE err in rcv");

    puts("Rcv from serv end");
    
    return serv_port;
}