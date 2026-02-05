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

void Server::CloseConnection(TcpSocket client_socket)
{
    std::cout << "Closing connection" << std::endl;
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_socket.GetSockfd(), NULL);
    client_socket.Close();
}


int Server::GetClient(std::string client_name)
{
    return _client_names.count(client_name) != 0 ? _client_names[client_name] : -1;
}

bool Server::SendMessage(Message message, int client)
{
    auto pair = _clients.find(client);
    return pair != _clients.end() && pair->second.Send(message.Serialize()) != -1;
}


bool Server::SendErrorMessage(std::string content, int client)
{
    return SendMessage({{"type", "error"}, {"content", content}}, client);
}

bool Server::SendSuccessMessage(std::string content, int client)
{
    return SendMessage({{"type", "success"}, {"content", content}}, client);
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
    if (channel.Receive(data) > 0)
    {
        if (message.Deserialize(data))
        {
            std::cout << "[Server] Received: " << message.ToString() << std::endl;
            HandleMessage(message, client_socket);
        }
        else
        {
            SendErrorMessage("Message received failed to deserialize.", client_socket.GetSockfd());
        }
    }
    else
    {
        CloseConnection(client_socket);
    }
}

bool Server::HandleMessage(Message message, TcpSocket client_socket)
{
    std::optional<std::string> type = message.Get("type");

         if (type == "login")   return HandleLoginMessage(message, client_socket);
    else if (type == "chat")    return HandleChatMessage(message, client_socket);

    SendErrorMessage("Message of unrecognized or missing type.", client_socket.GetSockfd());
    return false;
}

bool Server::HandleLoginMessage(Message message, TcpSocket client_socket)
{
    if (!message.Has("username"))
    {
        SendErrorMessage("Login message is missing username.", client_socket.GetSockfd());
        return false;
    }

    std::string username = message.Get("username").value();
    _client_names.emplace(username, client_socket.GetSockfd());

    message.Set("type", "logged in");
    return SendMessage(message, client_socket.GetSockfd());
}

bool Server::HandleChatMessage(Message message, TcpSocket client_socket)
{
    if (!message.HasAll("to", "content"))
    {
        SendErrorMessage("Chat message is missing username.", client_socket.GetSockfd());
        return false;
    }
    int client_fd = GetClient(message.Get("to").value());
    if (client_fd == -1)
        return false;
    return SendMessage(message, client_fd);
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
            std::cout << (events[i].events) << std::endl;
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