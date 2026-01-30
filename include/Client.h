#ifndef CLIENT_H
#define CLIENT_H

#include "TcpSocket.h"

class Client
{
    private:
    TcpSocket _socket;

    public:
    Client();
    ~Client();
    void Run();
};

#endif