#include "Server/Server.h"
#include "Shared/Global.h"
#include "Shared/Messages.h"
#include "Logging/Logger.h"
#include "Network/SecureChannel.h"
#include "Network/Message.h"
#include "Network/Serialization.h"

Server::Server()
    : _socket(PORT, INADDR_ANY)
    , _epoll_fd(epoll_create1(0)) // TODO: Handle errors
    , _message_buffer(MESSAGE_BUFFER_SIZE)
    , _max_requests_per_minute(120)
{
    _socket.Bind();
    _socket.Listen(1);
    _socket.SetNonBlocking();

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = _socket.GetFd();
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _socket.GetFd(), &ev); // TODO: Handle errors

    Logger::GetInstance().Info("[Server] Constructed");
}
Server::~Server()
{
    Logger::GetInstance().Info("[Server] Deconstructing");
}

void Server::Run()
{
    Logger::GetInstance().Info("[Server] Running");
    while(true) HandleEvents();
}

bool Server::HandleEvents()
{
    Logger::GetInstance().Trace("[Server] Checking epoll event queue");
    
    int num_events = epoll_wait(_epoll_fd, _epoll_events.data(), MAX_EVENTS, 100);
    for (int i = 0; i < num_events; ++i)
    {
        Logger::GetInstance().Trace("[Server] Handling event");
        int fd = _epoll_events[i].data.fd;

        if (fd == _socket.GetFd())
        {
            // Accept new connection
            Logger::GetInstance().Trace("[Server] Event coming from new client");
            int client_fd = _socket.Accept();
            if (client_fd != -1)
                EstablishConnection(client_fd);
        }
        else
        {
            Logger::GetInstance().Trace("[Server] Event coming from connected client");
            int client_fd = fd;

            // Check for secure channel between server and client
            if (_sessions.IsEstablished(client_fd))
            {
                // Receive across secure channel
                SecureChannel& channel = _sessions.GetChannel(client_fd);
                Logger::GetInstance().Trace("[Server] Receiving data across secure channel");

                // Check if this is a request or attempt to close the connection
                if (channel.Receive(_message_buffer) > 0)
                {
                    auto response (HandleBufferedRequest(client_fd));
                    auto& ref (ResponseRef(response));
                    if (SendResponse(channel, ref))
                        Logger::GetInstance().Info("[Server] Sent: \"" + ref.ToString() + "\"");
                    else
                        Logger::GetInstance().Error("[Server] Failed to send: \"" + ref.ToString() + "\"");
                }
                else
                    CloseConnection(client_fd);
            }
            else
                Logger::GetInstance().Error("[Server] Received request from host without secure connection: " + std::to_string(client_fd));
        }
    }

    return num_events > 0;
}

void Server::EstablishConnection(int client_fd)
{
    Logger::GetInstance().Info("[Server] Establishing connection " + std::to_string(client_fd));

    // Create secure session
    if (_sessions.Create(client_fd))
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

    _sessions.Destroy(client_fd);
}

bool Server::SendResponse(SecureChannel& channel, Message& message)
{
    return message.Serialize(_message_buffer) && channel.Send(_message_buffer) != -1;
}
bool Server::SendResponse(SecureChannel& channel, Message&& message)
{
    return message.Serialize(_message_buffer) && channel.Send(_message_buffer) != -1;
}

Response Server::HandleBufferedRequest(int client_fd)
{
    ByteReader reader (_message_buffer);
    uint8_t type_id;
    if (reader.Read(&type_id, sizeof(type_id)))
    {
        Logger::GetInstance().Trace("[Server] Request Type ID: \"" + std::to_string(type_id) + "\"");

        switch (type_id)
        {
            case Ping::TypeId:          return HandleBufferedRequestAs<Ping>(client_fd);
            case Register::TypeId:      return HandleBufferedRequestAs<Register>(client_fd);
            case Login::TypeId:         return HandleBufferedRequestAs<Login>(client_fd);  
            case SendChat::TypeId:      return HandleBufferedRequestAs<SendChat>(client_fd);
        }
        return Failure("Type ID unrecognized");
    }
    return Failure("Failed to read type ID");
}

template <typename T>
Response Server::HandleBufferedRequestAs(int client_fd)
{
    T request;
    if (request.Deserialize(_message_buffer))
    {
        Logger::GetInstance().Info("[Server] Received: \"" + request.ToString() + "\"");
        return HandleRequest<T>(client_fd, request);
    }
    return Failure("Request deserialization failed");
}
template Response Server::HandleBufferedRequestAs<Ping>(int client_fd);
template Response Server::HandleBufferedRequestAs<Login>(int client_fd);
template Response Server::HandleBufferedRequestAs<Register>(int client_fd);
template Response Server::HandleBufferedRequestAs<SendChat>(int client_fd);

template<>
Response Server::HandleRequest(int client_fd, Ping request)
{
    (void) client_fd;
    (void) request;

    return Success();
}

template<>
Response Server::HandleRequest(int client_fd, Register request)
{
    (void) client_fd;

    int error = _accounts.Register(request.username, request.password);
    if (error == 0)
        return Success();
    else
        return Failure(AccountRegistry::ErrorString(error));
}

template<>
Response Server::HandleRequest(int client_fd, Login request)
{
    if (_accounts.Contains(request.username) && _accounts.MatchingPassword(request.username, request.password))
        if (_sessions.Authenticate(client_fd, request.username))
            return Success();
        else
            return Failure("A user is already logged in under that account");
    else
        return Failure("Incorrect password or username");
}

template<>
Response Server::HandleRequest(int client_fd, SendChat request)
{
    if (_sessions.IsAuthenticated(client_fd))
        if (_sessions.IsEstablished(request.to))
            if (SendResponse(_sessions.GetChannel(request.to), ReceiveChat(_sessions.GetUsername(client_fd), request.content)))
                return Success();
            else
                return Failure("Chat failed to send");
        else
            return Failure("Recipient does not exist");
    else
        return Failure("Client unauthenticated");
}