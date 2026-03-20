#include "server/Server.h"
#include "shared/Global.h"
#include "logging/Logger.h"
#include "network/SecureChannel.h"
#include "network/Message.h"
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
{ }

void Server::Run()
{
    while(HandleEvents());
}

bool Server::HandleEvents()
{
    int num_events = epoll_wait(_epoll_fd, _epoll_events.data(), MAX_EVENTS, -1);
    if (num_events < 0)
        return false;

    for (int i = 0; i < num_events; ++i)
    {
        if (_epoll_events[i].data.fd == _socket.GetFd())
        { // Accept new connection
            int client_fd = _socket.Accept();
            if (client_fd == -1)
                continue;
            EstablishConnection(client_fd);
        }
        else
        { // Handle request
            HandleRequest(_epoll_events[i].data.fd);
        }
    }

    return true;
}

void Server::EstablishConnection(int client_fd)
{
    Logger::GetInstance().Info("[Server] Establishing connection " + std::to_string(client_fd));

    // Create secure session
    if (_sessions.CreateSession(client_fd))
    {
        // Add to epoll
        epoll_event ev {};
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = client_fd;
        epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev); // TODO: Handle errors
    }
}

void Server::CloseConnection(int client_fd)
{
    Logger::GetInstance().Info("[Server] Closing connection " + std::to_string(client_fd));

    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);

    _sessions.DestroySession(client_fd);
}

bool Server::SendMessage(SecureChannel& channel, Message message)
{
    bool success = channel.Send(message.Serialize()) != -1;
    if (success)
        Logger::GetInstance().Info("[Server] Sent message: \"" + message.ToString() + "\"");
    else
        Logger::GetInstance().Error("[Server] Failed to send message: \"" + message.ToString() + "\"");
    return success;
}

bool Server::SendErrorMessage(SecureChannel& channel, std::string content)
{
    return SendMessage(channel, {{"type", "error"}, {"content", content}});
}

bool Server::SendSuccessMessage(SecureChannel& channel, std::string content)
{
    return SendMessage(channel, {{"type", "success"}, {"content", content}});
}

void Server::HandleRequest(int client_fd)
{
    // Check for secure channel between server and client
    if (!_sessions.Has(client_fd))
    {
        Logger::GetInstance().Error("[Server] Received request from host without secure connection: " + std::to_string(client_fd));
        return;
    }

    // Receive message across secure channel
    Session& session = _sessions.Get(client_fd);
    Message message;
    std::vector<uint8_t> data;
    if (session.channel.Receive(data) > 0)
    {
        if (message.Deserialize(data))
        {
            Logger::GetInstance().Info("[Server] Received request from host " + std::to_string(client_fd) + " \"" + message.ToString() + "\"");
            HandleMessage(session, message);
        }
        else
        {
            Logger::GetInstance().Error("[Server] Received malformed request from host " + std::to_string(client_fd));
            SendErrorMessage(session.channel, "Message received failed to deserialize.");
        }
    }
    else
    {
        CloseConnection(client_fd);
    }
}

bool Server::HandleMessage(Session& session, Message message)
{
    std::optional<std::string> type = message.TryGet("type");

         if (type == "login")   return HandleLoginMessage(session, message);
    else if (type == "register")return HandleRegistrationMessage(session, message);
    else if (type == "chat")    return HandleChatMessage(session, message);

    SendErrorMessage(session.channel, "Message of unrecognized or missing type.");
    return false;
}


bool Server::HandleRegistrationMessage(Session& session, Message message)
{
    if (!message.Has("username"))
    {
        SendErrorMessage(session.channel, "Registration message is missing username.");
        return false;
    }
    if (!message.Has("password"))
    {
        SendErrorMessage(session.channel, "Registration message is missing password.");
        return false;
    }

    int error = _accounts.Register(message.Get("username"), message.Get("password"));
    if (error != 0)
    {
        SendErrorMessage(session.channel, AccountRegistry::ErrorString(error));
        return false;
    }

    message.Set("type", "registered");
    return SendMessage(session.channel, message);
}

bool Server::HandleLoginMessage(Session& session, Message message)
{
    if (!message.Has("username"))
    {
        SendErrorMessage(session.channel, "Login message is missing username.");
        return false;
    }
    if (!message.Has("password"))
    {
        SendErrorMessage(session.channel, "Login message is missing password.");
        return false;
    }

    const std::string& username = message.Get("username");
    if (!_accounts.Contains(username) || message.Get("password") != _accounts.GetPassword(username))
    {
        SendErrorMessage(session.channel, "Incorrect password or username.");
        return false;
    }
    if (_sessions.Has(username))
    {
        SendErrorMessage(session.channel, "A user is already logged in under that account.");
        return false;
    }

    _sessions.Authenticate(session.channel.GetSocket().GetFd(), username);

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
    if (!_sessions.Has(recipient)) {
        SendErrorMessage(session.channel, "Recipient does not exist.");
        return false;
    }

    return SendMessage(_sessions.Get(recipient).channel, message);
}