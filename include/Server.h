#ifndef SERVER_H
#define SERVER_H

#include "TcpSocket.h"
#include "SecureChannel.h"
#include "ThreadPool.h"
#include "Message.h"
#include <vector>
#include <thread>

class Server
{
    private:
    static constexpr std::size_t MAX_EVENTS = 1024;

    TcpSocket _socket;
    int _epoll_fd;
    ThreadPool _pool;
    std::map<int, SecureChannel> _clients;
    std::map<std::string, int> _client_names;

    void EstablishConnection(TcpSocket client_socket);
    void HandleRequest(TcpSocket client_socket);
    void HandleMessage(Message message);
    void EventLoop();

    public:
    Server();
    ~Server();
    void Run();
};

#endif