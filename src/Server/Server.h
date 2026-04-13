#ifndef SERVER_H
#define SERVER_H

#include "Server/AccountRegistry.h"
#include "Server/SessionManager.h"
#include "Network/TcpSocket.h"
#include "Network/SecureChannel.h"
#include "Network/Message.h"
#include "Shared/ThreadPool.h"
#include <vector>
#include <thread>
#include <optional>
#include <array>
#include <sys/epoll.h>

class Server
{
    private:
    static constexpr std::size_t MAX_EVENTS = 1024;

    TcpSocket _socket;
    int _epoll_fd;
    std::array<epoll_event, MAX_EVENTS> _epoll_events;

    ThreadPool _pool;
    SessionManager _sessions;
    AccountRegistry _accounts;

    int _max_requests_per_minute;

    void EstablishConnection(int client_fd);
    void CloseConnection(int client_fd);

    bool SendMessage(SecureChannel& channel, Message message);

    void HandleRequest(int client_fd);
    Message HandleMessage(int client_fd, Message message);
    Message HandleRegistrationMessage(int client_fd, Message message);
    Message HandleLoginMessage(int client_fd, Message message);
    Message HandleChatMessage(int client_fd, Message message);

    public:
    Server();
    ~Server();
    void Run();
    bool HandleEvents();
};

#endif