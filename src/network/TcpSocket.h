#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <iostream>
#include <vector>
#include <cstdint>
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
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket(TcpSocket&& other);
    ~TcpSocket();

    void CheckSyscall(int result, std::string error_message);

    int SetNonBlocking();

    int Bind();
    int Connect();
    int Listen(int backlog);
    int Accept();
    int Send(const void* data, std::size_t size, int flags);
    // TODO: SendAll();
    int SendBytes(const std::vector<uint8_t>& bytes);
    int Receive(void* data, std::size_t size, int flags);
    int ReceiveAll(void* data, std::size_t size);
    int ReceiveBytes(std::vector<uint8_t>& bytes);

    struct sockaddr_in GetAddr() const;
    int GetFd() const;
};

#endif