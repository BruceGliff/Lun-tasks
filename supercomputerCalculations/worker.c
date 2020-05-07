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
void establishConnection(int port, struct sockaddr_in * server);
int rcvPortFromServer();

int main()
{
    broadcastToServer();
    struct sockaddr_in server;
    int port = rcvPortFromServer(&server);
    printf("%d\n", port);
    establishConnection(port, &server);


    puts("before sleep");
    sleep(1);
    puts("after sleep");

    
    
}
void establishConnection(int port, struct sockaddr_in * server)
{
    puts("begin establish connection");
    int sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk < 0) 
        perror("ERROR opening socket");

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
    memcpy(&serverAddr.sin_addr, &server->sin_addr, sizeof(serverAddr.sin_addr));
    serverAddr.sin_port = htons(port);
    
    if (connect(sk, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) 
        perror("ERROR connecting");

    char b[100];
    int n = read(sk, b, 2);
    if (n < 0) 
        perror("ERROR writing to socket");

    puts("end establish connection");
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

int rcvPortFromServer(struct sockaddr_in * serv_addr)
{
    puts("Rcv from server begin");
    struct sockaddr_in my_addr;
    int recaddrlen = sizeof(struct sockaddr_in);

    int port = 5;
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
            ERROR("Err bind");
    }

    int serv_port;

    int l = sizeof(struct sockaddr_in);
    int res = recvfrom(bcast_sock, &serv_port, 4, 0, (struct sockaddr *)serv_addr, &l);
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