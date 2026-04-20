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
            Logger::GetInstance().Trace("[Server] Event coming from connected client");
            int client_fd = _epoll_events[i].data.fd;

            // Check for secure channel between server and client
            if (!_sessions.IsEstablished(client_fd))
            {
                Logger::GetInstance().Error("[Server] Received request from host without secure connection: " + std::to_string(client_fd));
                continue;
            }

            // Receive across secure channel
            SecureChannel& channel = _sessions.GetChannel(client_fd);
            Logger::GetInstance().Trace("[Server] Receiving data across secure channel");

            // Check if this is a request or attempt to close the connection
            if (channel.Receive(_message_buffer) > 0)
            {
                if (_sessions.GetRequestsLastMinute(client_fd) < _max_requests_per_minute) {
                    _sessions.SubmitRequestTimestamp(client_fd);
                    HandleRequest(_epoll_events[i].data.fd);
                }
                else
                {
                    std::string error = "Rate limit of " + std::to_string(client_fd) + " exceeded (" + std::to_string(_max_requests_per_minute) + " requests per minute";
                    Logger::GetInstance().Error("[Server] " + error);
                    SendResponse(channel, Failure(error));
                }
            }
            else
            {
                CloseConnection(client_fd);
            }
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

bool Server::SendResponse(SecureChannel& channel, Message&& message)
{
    
    bool success = message.Serialize(_message_buffer) && channel.Send(_message_buffer) != -1;
    if (success)
        Logger::GetInstance().Info("[Server] Sent response: \"" + message.ToString() + "\"");
    else
        Logger::GetInstance().Error("[Server] Failed to send response: \"" + message.ToString() + "\"");
    return success;
}

void Server::HandleRequest(int client_fd)
{
    bool parsed = false;
    ByteReader reader (_message_buffer);
    uint8_t type_id;
    if (reader.Read(&type_id, sizeof(type_id)))
    {
        Logger::GetInstance().Trace("[Server] Request Type ID: \"" + std::to_string(type_id) + "\"");
        if (type_id == Register::TypeId)
        {
            Register request;
            if (request.Deserialize(_message_buffer))
            {
                Logger::GetInstance().Info("[Server] Received Request: \"" + request.ToString() + "\"");
                RegisterAccount(client_fd, request);
                parsed = true;
            }
        }
        else if (type_id == Login::TypeId)
        {
            Login request;
            if (request.Deserialize(_message_buffer))
            {
                Logger::GetInstance().Info("[Server] Received Request: \"" + request.ToString() + "\"");
                LoginClient(client_fd, request);
                parsed = true;
            }
        }
        else if (type_id == SendChat::TypeId)
        {
            SendChat request;
            if (request.Deserialize(_message_buffer))
            {
                Logger::GetInstance().Info("[Server] Received Request: \"" + request.ToString() + "\"");
                ForwardChat(client_fd, request);
                parsed = true;
            }
        }
    }

    if (!parsed)
        SendResponse(_sessions.GetChannel(client_fd), Failure("Failed to parse request"));
}

void Server::RegisterAccount(int client_fd, Register request)
{
    int error = _accounts.Register(request.username, request.password);
    if (error == 0)
        SendResponse(_sessions.GetChannel(client_fd), Success());
    else
        SendResponse(_sessions.GetChannel(client_fd), Failure(AccountRegistry::ErrorString(error)));
}

void Server::LoginClient(int client_fd, Login request)
{
    auto& channel = _sessions.GetChannel(client_fd);

    if (_accounts.Contains(request.username) && _accounts.MatchingPassword(request.username, request.password))
        if (_sessions.Authenticate(client_fd, request.username))
            SendResponse(channel, Success());
        else
            SendResponse(channel, Failure("A user is already logged in under that account"));
    else
        SendResponse(channel, Failure("Incorrect password or username"));
}

void Server::ForwardChat(int client_fd, SendChat request)
{
    auto& channel = _sessions.GetChannel(client_fd);

    if (_sessions.IsAuthenticated(client_fd))
        if (_sessions.IsEstablished(request.to))
            if (SendResponse(_sessions.GetChannel(request.to), ReceiveChat(_sessions.GetUsername(client_fd), request.content)))
                SendResponse(channel, Success());
            else
                SendResponse(channel, Failure("Chat failed to send"));
        else
            SendResponse(channel, Failure("Recipient does not exist"));
    else
        SendResponse(channel, Failure("Client unauthenticated"));
}