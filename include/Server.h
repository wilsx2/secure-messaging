#ifndef SERVER_H
#define SERVER_H

#include "TcpSocket.h"
#include "ThreadPool.h"
#include <vector>
#include <thread>

class Server
{
    private:
    TcpSocket _socket;
    ThreadPool _pool;

    void HandleConnection(TcpSocket socket);

    public:
    Server();
    ~Server();
    void Run();
};

#endif