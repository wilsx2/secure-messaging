#ifndef CLIENT_H
#define CLIENT_H

#include "SecureChannel.h"
#include "TcpSocket.h"

class Client
{
    private:
    TcpSocket _socket;
    void ReceiveLoop(SecureChannel& channel);

    public:
    Client();
    ~Client();
    void Run();
};

#endif