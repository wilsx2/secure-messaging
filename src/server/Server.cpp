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

    Logger::GetInstance().Info("[Server] Created");
}
Server::~Server()
{
    Logger::GetInstance().Info("[Server] Deconstructing");
}

void Server::Run()
{
    Logger::GetInstance().Info("[Server] Running");
    while(HandleEvents());
}

bool Server::HandleEvents()
{
    int num_events = epoll_wait(_epoll_fd, _epoll_events.data(), MAX_EVENTS, -1);
    if (num_events < 0)
        return false;

    for (int i = 0; i < num_events; ++i)
    {
        Logger::GetInstance().Trace("[Server] Handling event");
        if (_epoll_events[i].data.fd == _socket.GetFd())
        {
            // Accept new connection
            Logger::GetInstance().Trace("[Server] Event coming from new client");
            int client_fd = _socket.Accept();
            if (client_fd == -1)
                continue;
            EstablishConnection(client_fd);
        }
        else
        {
            // Handle request
            Logger::GetInstance().Trace("[Server] Event coming from connected client");
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
    Logger::GetInstance().Trace("[Server] Receiving data across secure channel");
    if (session.channel.Receive(data) > 0)
    {
        if (message.Deserialize(data))
        {
            // Process request and send response
            Logger::GetInstance().Info("[Server] Received request from host " + std::to_string(client_fd) + " \"" + message.ToString() + "\"");
            Message response = HandleMessage(session, message);
            SendMessage(session.channel, response);
        }
        else
        {
            Logger::GetInstance().Error("[Server] Received malformed request from host " + std::to_string(client_fd));
            SendMessage(session.channel, Message::Error("Message received failed to deserialize"));
        }
    }
    else
    {
        CloseConnection(client_fd);
    }
}

Message Server::HandleMessage(Session& session, Message message)
{
    std::optional<std::string> type = message.TryGet("type");

         if (type == "login")       return HandleLoginMessage(session, message);
    else if (type == "register")    return HandleRegistrationMessage(session, message);
    else if (type == "chat")        return HandleChatMessage(session, message);
    return Message::Error("Message of unrecognized type");
}


Message Server::HandleRegistrationMessage(Session& session, Message message)
{
    (void) session; // Unused

    // Validate
    if (!message.HasAll("username", "password"))
        return Message::Error("Registration message is incomplete");

    // Attempt Register
    int error = _accounts.Register(message.Get("username"), message.Get("password"));
    if (error != 0)
        return Message::Error(AccountRegistry::ErrorString(error));

    // Successful Response
    message.Set("type", "registered");
    return message;
}

Message Server::HandleLoginMessage(Session& session, Message message)
{
    // Validate
    if (!message.HasAll("username", "password"))
        return Message::Error("Login message is incomplete");

    const std::string& username = message.Get("username");
    if (!_accounts.Contains(username) || message.Get("password") != _accounts.GetPassword(username))
        return Message::Error("Incorrect password or username.");

    if (_sessions.Has(username))
        return Message::Error("A user is already logged in under that account");

    // Authenticate
    _sessions.Authenticate(session.channel.GetSocket().GetFd(), username);

    // Successful Response
    message.Set("type", "logged in");
    return message;
}

Message Server::HandleChatMessage(Session& session, Message message)
{
    // Validate
    if (!message.HasAll("to", "content"))
        return Message::Error("Chat message is incomplete");

    if (!session.authenticated)
        return Message::Error("Sender is unauthenticated");

    const std::string& recipient = message.Get("to");
    if (!_sessions.Has(recipient))
        return Message::Error("Recipient does not exist");

    // Attempt Send
    message.Set("from", session.username);
    if (!SendMessage(_sessions.Get(recipient).channel, message))
        return Message::Error("Message failed to send");

    // Successful Response
    message.Set("type", "sent");
    return message;
}