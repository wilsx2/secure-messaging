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
    ev.data.fd = _socket.GetFd();
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _socket.GetFd(), &ev); // TODO: Handle errors
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
    ev.data.fd = client_socket.GetFd();
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_socket.GetFd(), &ev); // TODO: Handle errors
    
    // Create Secure Channel
    SecureChannel channel (client_socket, HostType::Server);
    _sessions.emplace(client_socket.GetFd(), channel); // TODO: Move semantics
}

void Server::CloseConnection(TcpSocket client_socket)
{
    std::cout << "Closing connection" << std::endl;
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_socket.GetFd(), NULL);
    client_socket.Close();
}

bool Server::SendMessage(SecureChannel& channel, Message message)
{
    return channel.Send(message.Serialize()) != -1;
}

bool Server::SendErrorMessage(SecureChannel& channel, std::string content)
{
    return SendMessage(channel, {{"type", "error"}, {"content", content}});
}

bool Server::SendSuccessMessage(SecureChannel& channel, std::string content)
{
    return SendMessage(channel, {{"type", "success"}, {"content", content}});
}

void Server::HandleRequest(TcpSocket client_socket)
{
    std::cout << "Request received" << std::endl;

    // Check for secure channel between server and client
    auto it = _sessions.find(client_socket.GetFd());
    if (it == _sessions.end())
        //TODO: Error handling
        return;

    // Receive message across secure channel
    SecureChannel& session = it->second;
    SecureChannel& channel = session;
    Message message;
    std::vector<uint8_t> data;        
    if (channel.Receive(data) > 0)
    {
        if (message.Deserialize(data))
        {
            std::cout << "[Server] Received: " << message.ToString() << std::endl;
            HandleMessage(session, message);
        }
        else
        {
            SendErrorMessage(session, "Message received failed to deserialize.");
        }
    }
    else
    {
        CloseConnection(client_socket);
    }
}

bool Server::HandleMessage(SecureChannel& session, Message message)
{
    std::optional<std::string> type = message.Get("type");

         if (type == "login")   return HandleLoginMessage(session, message);
    else if (type == "chat")    return HandleChatMessage(session, message);

    SendErrorMessage(session, "Message of unrecognized or missing type.");
    return false;
}

bool Server::HandleLoginMessage(SecureChannel& session, Message message)
{
    if (!message.Has("username"))
    {
        SendErrorMessage(session, "Login message is missing username.");
        return false;
    }

    std::string username = message.Get("username").value();
    _user_to_socket.emplace(username, session.GetSocket().GetFd());

    message.Set("type", "logged in");
    return SendMessage(session, message);
}

bool Server::HandleChatMessage(SecureChannel& session, Message message)
{
    if (!message.HasAll("to", "content"))
    {
        SendErrorMessage(session, "Chat message is missing username.");
        return false;
    }
    const std::string& recipient = message.Get("to").value();
    auto user_it = _user_to_socket.find(recipient);
    if (user_it == _user_to_socket.end())
        return false;

    auto session_it = _sessions.find(user_it->second);
    if (session_it == _sessions.end())
        return false;

    return SendMessage(session_it->second, message);
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
            if (events[i].data.fd == _socket.GetFd())
            { // Accept new connection
                TcpSocket client_socket = _socket.Accept();
                if (client_socket.GetFd() == -1)
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