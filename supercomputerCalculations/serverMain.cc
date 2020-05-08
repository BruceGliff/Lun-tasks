#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include<fcntl.h> 

#include "SWorkerReciever.h"
#include "STaskSender.h"
#include "Task.h"

#define ERROR(a)    \
    {               \
        perror(a);  \
        exit(-1);   \
    }

int establishConnection(int port);
void * acceptc (void * data);
int free_indx = 0;
pthread_mutex_t mutex;
int listen_sk = 0;

struct pointery
{
    TasksQueue * q;
    pthread_t * t;
};

int main()
{
    pthread_mutex_init(&mutex, NULL);
    //create pipe for structures and pthread
    int const size_addr = sizeof(sockaddr_in);

    pthread_t sync_th;
    if (pthread_create(&sync_th, NULL, reciever, NULL) != 0)
        ERROR("Err create thread");


    sockaddr_in worker_addr;
    int port = 102;

    pthread_t th_queue[256];


    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
        ERROR("Err create pipe");
    
    TasksQueue q(pipe_fd[1]);

    listen_sk = establishConnection(100);
    listen(listen_sk, 256);

    double res = 0;
    pointery p = {&q, th_queue};       

        pthread_t acc;
        pthread_create(&acc, NULL, acceptc, &p);

    int reslen = read(pipe_fd[0], &res, sizeof(double));

    pthread_mutex_lock(&mutex);
    if (shutdown(listen_sk, SHUT_RDWR) == -1);
        perror(0);
    close(listen_sk);
    close(pipe_fd[0]);
    pthread_cancel(acc);
    for (int i = 0; i != free_indx; ++i)
        pthread_join(th_queue[i], NULL);

    printf("RESULT: %e\n", res);

    pthread_mutex_unlock(&mutex);

    return 0;
}


void * acceptc (void * data)
{
    pointery * p = (pointery *) data;
    while(1)
    {       
        int newSocket = accept(listen_sk, NULL, NULL);
        if (newSocket == -1)
            return NULL;
        pthread_mutex_lock(&mutex);
        ConnectionSettings set = {p->q, newSocket};
        if (pthread_create(&(p->t[free_indx++]), NULL, TaskSender, &set) != 0)
            ERROR("Err create thread");

        pthread_mutex_unlock(&mutex);
    }
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


