#pragma once

#include "IpAddress.h"
#include <vector>
#include <expected>
#include <cstdint>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

class TcpSocket
{
    public:
    protected:
    int _sockfd;
    IpAddress _address;
    int _port;

    public:
    enum class Error { NotReady , Disconnected , Unexpected };
    using Handle = int;

    TcpSocket();
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator= (const TcpSocket&) = delete;
    TcpSocket(TcpSocket&& socket);
    ~TcpSocket();

    void Close();

    void SetBlocking(bool blocking);
    bool IsBlocking() const;

    Handle GetHandle() const;
};