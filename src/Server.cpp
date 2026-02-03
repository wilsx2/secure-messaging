#include "Global.h"
#include "Server.h"
#include "SecureChannel.h"
#include "Message.h"
#include <sys/epoll.h>

Server::Server()
    : _socket(PORT, INADDR_ANY)
    , _epoll_fd(epoll_create1(0)) // TODO: Handle errors
{
    _socket.Bind();
    _socket.Listen(1);
    _socket.SetNonBlocking();

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = _socket.GetSockfd();
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _socket.GetSockfd(), &ev); // TODO: Handle errors
}
Server::~Server()
{
    _socket.Close();
}

void Server::EstablishConnection(TcpSocket client_socket)
{
    std::cout << "Establishing connection" << std::endl;

    // Add to epoll
    epoll_event ev {};
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_socket.GetSockfd();
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_socket.GetSockfd(), &ev); // TODO: Handle errors
    
    // Create Secure Channel
    SecureChannel channel (client_socket, HostType::Server);
    _clients.emplace(client_socket.GetSockfd(), channel);
}

void Server::HandleRequest(TcpSocket client_socket)
{
    std::cout << "Request received" << std::endl;

    // Check for secure channel between server and client
    int client_fd = client_socket.GetSockfd();
    auto client_pair = _clients.find(client_fd);
    if (client_pair == _clients.end())
        return;

    // Receive message across secure channel
    SecureChannel& channel = client_pair->second;
    Message message;
    std::vector<uint8_t> data;        
    if (channel.Receive(data) > 0 && message.Deserialize(data))
    {
        std::cout << "[Server] Received: " << message.ToString() << std::endl;
    }

    // Handle message
    bool successful = HandleMessage(message, client_socket);
    /// Send response
    Message response;
    response.Set("type", "response");
    response.Set("success", successful ? "true" : "false");
    channel.Send(response.Serialize());
}

bool Server::HandleMessage(Message message, TcpSocket client_socket)
{
    std::optional<std::string> type = message.Get("type");

         if (type == "login")   return HandleLoginMessage(message, client_socket);
    else if (type == "chat")    return HandleChatMessage(message, client_socket);

    return false;
}

bool Server::HandleLoginMessage(Message message, TcpSocket client_socket)
{
    std::optional<std::string> username = message.Get("username");

    if (username.has_value())
    {
        _client_names.emplace(username.value(), client_socket.GetSockfd());
        return true;
    }
    return false;
}

bool Server::HandleChatMessage(Message message, TcpSocket client_socket)
{
    // TODO: Check and store from
    std::optional<std::string> to       = message.Get("to");
    std::optional<std::string> content  = message.Get("content");

    if (to.has_value() && content.has_value())
    {
        if (_client_names.count(to.value()) > 0)
        {
            int recipient_fd = _client_names.at(to.value());
            auto recipient_pair = _clients.find(recipient_fd);
            if (recipient_pair != _clients.end())
            {
                recipient_pair->second.Send(message.Serialize());
                return true;
            }
        }
    }
    return false;
}

void Server::EventLoop()
{
    std::array<epoll_event, MAX_EVENTS> events;

    while(true)
    {
        int num_events = epoll_wait(_epoll_fd, events.data(), MAX_EVENTS, -1);
        if (num_events < 0)
            break;

        for (int i = 0; i < num_events; ++i)
        {
            if (events[i].data.fd == _socket.GetSockfd())
            { // Accept new connection
                TcpSocket client_socket = _socket.Accept();
                if (client_socket.GetSockfd() == -1)
                    continue;
                EstablishConnection(client_socket);
            }
            else
            { // Handle request
                HandleRequest(events[i].data.fd);
            }
        }
    }
}

void Server::Run()
{
    EventLoop();
}