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
#include <arpa/inet.h>

void castToWorker(struct sockaddr_in a);
struct sockaddr_in rcvFromWorker();

void establichConnection(struct sockaddr_in);

int main()
{
    establichConnection(rcvFromWorker());



    puts("before sleep");
    sleep(1);
    puts("after sleep");
}

void establichConnection(struct sockaddr_in workerAddr_)
{
    int sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk < 0) 
        perror("ERROR opening socket");

	struct sockaddr_in workerAddr;
	memcpy(&workerAddr, &workerAddr_, sizeof(workerAddr));
	workerAddr.sin_port = htons(101);
    
    if (connect(sk, (struct sockaddr *) &workerAddr, sizeof(workerAddr)) < 0) 
        perror("ERROR connecting");

    int n = write(sk, "EC", 2);
    if (n < 0) 
        perror("ERROR writing to socket");

    puts("end establish connection");
    sleep(1);
}

void castToWorker(struct sockaddr_in workaddr)
{
    puts("CastBegin");

    int bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcast_sock < 0)
        perror("cannot create sock for broadcast\n");

    int broadcastEnable = 1;
    //int ret = setsockopt(bcast_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    //if (ret < 0)    
        //perror("cannot set up socket for broadcast\n");

    char mess[] = "TOWORKER";
    if(sendto(bcast_sock, mess, strlen(mess), 0, (struct sockaddr *) &workaddr, sizeof(workaddr) < 0))
        perror("sendto");

    close(bcast_sock);
    puts("CastEnd");

    return;
}

struct sockaddr_in rcvFromWorker()
{
    puts("Rcv from worker begin");
    struct sockaddr_in serv_addr;
    struct sockaddr_in rec_addr; int recaddrlen = sizeof(rec_addr);
    int port = 101;
    int bcast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcast_sock < 0)
        perror("cannot create sock rcvFromServer\n");
    int broadcastEnable=1;
    if(setsockopt(bcast_sock,SOL_SOCKET,SO_BROADCAST,&broadcastEnable,sizeof(int)) == -1)
    {
            perror("setsockopt");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_BROADCAST; // ANY
    serv_addr.sin_port = htons(port);

    if(bind(bcast_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))==-1)
    {
            close(bcast_sock);
            perror("cannot bind");
    }

    while(1){
    char recvString[100];
    memset(&rec_addr, 0, recaddrlen);
    int res = recvfrom(bcast_sock, recvString, 99, 0, (struct sockaddr *) &rec_addr, &recaddrlen);
    if (res < 0)
    {
        perror("cannot recieve");
    }

    recvString[res] = '\0';
    printf("Accept: %s\n", recvString);

    if(close(bcast_sock) == -1)
        perror("CLOSE err in rcv");

    printf("Datagram size: %d.\n", res);
    printf("Datagram's IP address is: %s\n", inet_ntoa(rec_addr.sin_addr));
    printf("Datagram's port is: %d\n", (int) ntohs(rec_addr.sin_port));

    puts("Rcv from serv end");
    }
    return rec_addr;
}
