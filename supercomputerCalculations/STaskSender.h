#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


struct ConnectSettings
{
    struct sockaddr_in worker_addr;
    int port;
};