#ifndef SERVER_H
#define SERVER_H

#include "TcpSocket.h"
#include <vector>
#include <thread>

class Server
{
    private:
    std::vector<std::thread> _threads;
    TcpSocket _socket;

    void HandleConnection(TcpSocket socket);

    public:
    Server();
    void Run();
};

#endif