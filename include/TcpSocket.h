#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <iostream>
#include <vector>
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
    TcpSocket(int sockfd);
    void Close();

    void CheckSyscall(int result, std::string error_message);

    int SetNonBlocking();

    int Bind();
    int Connect();
    int Listen(int backlog);
    TcpSocket Accept();
    int Send(const void* data, std::size_t size);
    int Receive(void* data, std::size_t size);

    struct sockaddr_in GetAddr();
    int GetSockfd();
};

#endif