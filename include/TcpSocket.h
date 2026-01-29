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
    struct sockaddr_in m_address;
    int m_sock;

    public:
    TcpSocket(int port, u_long interface);
    ~TcpSocket();
    void TestConnection(int item);
    int Bind();
    int Connect();
    int Listen(int backlog);
    struct sockaddr_in GetAddress();
    int GetSock();
};

#endif