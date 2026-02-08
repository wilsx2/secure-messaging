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

    // Create secure session
    Session session { SecureChannel(client_socket), "", false };
    session.channel.EstablishKey(HostType::Server);
    _sessions.emplace(client_socket.GetFd(), session); // TODO: Move semantics

    // Add to epoll
    epoll_event ev {};
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_socket.GetFd();
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_socket.GetFd(), &ev); // TODO: Handle errors
}

void Server::CloseConnection(TcpSocket client_socket)
{
    std::cout << "Closing connection" << std::endl;
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_socket.GetFd(), NULL);
    client_socket.Close();

    auto it = _sessions.find(client_socket.GetFd());
    if (it == _sessions.end())
        return;
    _user_to_socket.erase(it->second.username);
    _sessions.erase(client_socket.GetFd());
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
    Session& session = it->second;
    Message message;
    std::vector<uint8_t> data;
    if (session.channel.Receive(data) > 0)
    {
        if (message.Deserialize(data))
        {
            std::cout << "[Server] Received: " << message.ToString() << std::endl;
            HandleMessage(session, message);
        }
        else
        {
            SendErrorMessage(session.channel, "Message received failed to deserialize.");
        }
    }
    else
    {
        CloseConnection(client_socket);
    }
}

bool Server::HandleMessage(Session& session, Message message)
{
    std::optional<std::string> type = message.TryGet("type");

         if (type == "login")   return HandleLoginMessage(session, message);
    else if (type == "chat")    return HandleChatMessage(session, message);

    SendErrorMessage(session.channel, "Message of unrecognized or missing type.");
    return false;
}

bool Server::HandleLoginMessage(Session& session, Message message)
{
    if (!message.Has("username"))
    {
        SendErrorMessage(session.channel, "Login message is missing username.");
        return false;
    }

    const std::string& username = message.Get("username");
    if (_user_to_socket.contains(username))
    {
        SendErrorMessage(session.channel, "User of that name is already logged in.");
        return false;
    }

    session.username = username;
    session.authenticated = true;
    _user_to_socket.emplace(username, session.channel.GetSocket().GetFd());

    message.Set("type", "logged in");
    return SendMessage(session.channel, message);
}

bool Server::HandleChatMessage(Session& session, Message message)
{
    if (!message.HasAll("to", "content"))
    {
        SendErrorMessage(session.channel, "Chat message is missing recipient.");
        return false;
    }
    if (!session.authenticated)
    {
        SendErrorMessage(session.channel, "Sender is unauthenticated.");
        return false;
    }

    message.Set("from", session.username);
    const std::string& recipient = message.Get("to");
    auto user_it = _user_to_socket.find(recipient);
    if (user_it == _user_to_socket.end())
        return false;

    auto session_it = _sessions.find(user_it->second);
    if (session_it == _sessions.end())
        return false;

    return SendMessage(session_it->second.channel, message);
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