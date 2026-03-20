#ifndef SERVER_H
#define SERVER_H

#include "server/AccountRegistry.h"
#include "server/SessionManager.h"
#include "network/TcpSocket.h"
#include "network/SecureChannel.h"
#include "network/Message.h"
#include "shared/ThreadPool.h"
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

    void EstablishConnection(int client_fd);
    void CloseConnection(int client_fd);

    bool SendMessage(SecureChannel& channel, Message message);

    void HandleRequest(int client_fd);
    Message HandleMessage(Session& session, Message message);
    Message HandleRegistrationMessage(Session& session, Message message);
    Message HandleLoginMessage(Session& session, Message message);
    Message HandleChatMessage(Session& session, Message message);

    public:
    Server();
    ~Server();
    void Run();
    bool HandleEvents();
};

#endif