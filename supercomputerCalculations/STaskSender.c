#include <unistd.h>


#include "STaskSender.h"


#define ERROR(a)    \
    {               \
        perror(a);  \
        exit(-1);   \
    }

int establichConnection(int port, struct sockaddr_in * workerAddr_);

void * computerHandler(void * data)
{
    struct ConnectSettings * settings = (struct ConnectSettings *) data;
    int port = settings->port;
    
    int sk = establichConnection(settings->port, &settings->worker_addr);
    
    int i = 3;
    while(i == 3)
    {
        write(sk, "TasK", 4);
        char ans[4];
        ans[3] = 0;
        i =read(sk, ans, 3);
    }

    if(shutdown(sk, SHUT_RDWR) == -1)
        ERROR("Err shutdown connection");   

    return NULL;
}

int establichConnection(int port, struct sockaddr_in * workerAddr)
{
    puts("Start establish cpnnecton");
    int sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk < 0) 
        ERROR("ERROR opening socket");


	workerAddr->sin_port = htons(101);
    
    if (connect(sk, (struct sockaddr *) workerAddr, sizeof(struct sockaddr_in)) < 0) 
        ERROR("ERROR connecting");

    return sk;
}