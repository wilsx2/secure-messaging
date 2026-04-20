#pragma once

#include "Server/AccountRegistry.h"
#include "Server/SessionManager.h"
#include "Network/TcpSocket.h"
#include "Network/SecureChannel.h"
#include "Network/Message.h"
#include "Shared/ThreadPool.h"
#include "Shared/Messages.h"
#include <vector>
#include <thread>
#include <optional>
#include <array>
#include <memory>
#include <sys/epoll.h>

class Server
{
    private:
    static constexpr std::size_t MAX_EVENTS = 1024;

    TcpSocket _socket;
    int _epoll_fd;
    std::array<epoll_event, MAX_EVENTS> _epoll_events;
    std::vector<uint8_t> _message_buffer;

    ThreadPool _pool;
    SessionManager _sessions;
    AccountRegistry _accounts;

    int _max_requests_per_minute;

    void EstablishConnection(int client_fd);
    void CloseConnection(int client_fd);

    bool SendResponse(SecureChannel& channel, Message& message);
    bool SendResponse(SecureChannel& channel, Message&& message);

    std::unique_ptr<Message> HandleBufferedRequest(int client_fd);
    template <typename T>
    std::unique_ptr<Message> HandleBufferedRequestAs(int client_fd);
    template <typename T>
    std::unique_ptr<Message> HandleRequest(int client_fd, T request);

    public:
    Server();
    ~Server();
    void Run();
    bool HandleEvents();
};