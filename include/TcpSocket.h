#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

class TcpSocket
{
    protected:
    struct sockaddr_in _addr;
    int _sockfd;

    public:
    TcpSocket(int port, u_long interface);
    TcpSocket(int sockfd);
    ~TcpSocket();
    void TestConnection(int item);
    int Bind();
    int Connect();
    int Listen(int backlog);
    TcpSocket Accept();
    struct sockaddr_in GetAddr();
    int GetSockfd();
};

#endif