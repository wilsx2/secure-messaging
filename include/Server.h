#ifndef SERVER_H
#define SERVER_H

#include "TcpSocket.h"
#include "SecureChannel.h"
#include "ThreadPool.h"
#include "Message.h"
#include <vector>
#include <thread>
#include <optional>
#include <array>
#include <sys/epoll.h>


struct Session
{
    SecureChannel channel;
    std::string username;
    bool authenticated;
};

class Server
{
    private:
    static constexpr std::size_t MAX_EVENTS = 1024;

    TcpSocket _socket;
    int _epoll_fd;
    std::array<epoll_event, MAX_EVENTS> _epoll_events;

    ThreadPool _pool;
    std::map<int, Session> _sessions;
    std::map<std::string, int> _user_to_socket;

    bool SendMessage(SecureChannel& channel, Message message);
    bool SendErrorMessage(SecureChannel& channel, std::string content);
    bool SendSuccessMessage(SecureChannel& channel, std::string content);

    void EstablishConnection(int client_fd);
    void CloseConnection(int client_fd);

    void HandleRequest(int client_fd);
    bool HandleMessage(Session& session, Message message);
    bool HandleLoginMessage(Session& session, Message message);
    bool HandleChatMessage(Session& session, Message message);

    public:
    Server();
    ~Server();
    void Run();
    bool HandleEvents();
};

#endif