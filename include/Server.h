#ifndef SERVER_H
#define SERVER_H

#include "TcpSocket.h"
#include "SecureChannel.h"
#include "ThreadPool.h"
#include "Message.h"
#include <vector>
#include <thread>
#include <optional>

class Server
{
    private:
    static constexpr std::size_t MAX_EVENTS = 1024;

    TcpSocket _socket;
    int _epoll_fd;
    ThreadPool _pool;
    std::map<int, SecureChannel> _sessions;
    std::map<std::string, int> _user_to_socket;
    
    bool SendMessage(SecureChannel& channel, Message message);
    bool SendErrorMessage(SecureChannel& channel, std::string content);
    bool SendSuccessMessage(SecureChannel& channel, std::string content);

    void EstablishConnection(TcpSocket client_socket);
    void CloseConnection(TcpSocket client_socket);
    void HandleRequest(TcpSocket client_socket);
    bool HandleMessage(SecureChannel& session, Message message);
    bool HandleLoginMessage(SecureChannel& session, Message message);
    bool HandleChatMessage(SecureChannel& session, Message message);

    void EventLoop();

    public:
    Server();
    ~Server();
    void Run();
};

#endif