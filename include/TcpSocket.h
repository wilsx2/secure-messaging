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
    TcpSocket(int sockfd, struct sockaddr_in addr);
    ~TcpSocket();
    void TestConnection(int item);
    int Bind();
    int Connect();
    int Listen(int backlog);
    TcpSocket Accept();
    void Write(std::string message);
    std::string Read(std::size_t buffer_size);
    struct sockaddr_in GetAddr();
    int GetSockfd();
};

#endif