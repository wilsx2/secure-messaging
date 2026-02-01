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

void Server::Run()
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
            { // Accept new connect
                std::cout << "Establishing connection" << std::endl;

                // Accept socket
                TcpSocket client_socket = _socket.Accept();
                if (client_socket.GetSockfd() == -1)
                    continue;
                
                // Add to epoll
                epoll_event ev {};
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_socket.GetSockfd();
                epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_socket.GetSockfd(), &ev); // TODO: Handle errors
                
                // Create Secure Channel
                SecureChannel sc (client_socket, HostType::Server);
                _clients.emplace(client_socket.GetSockfd(), sc);
            }
            else
            { // Handle request
                std::cout << "Request received" << std::endl;

                int client_fd = events[i].data.fd;
                auto client_pair = _clients.find(client_fd);
                if (client_pair == _clients.end())
                    continue;

                SecureChannel& channel = client_pair->second;
                Message message;
                std::vector<uint8_t> data;        
                if (channel.Receive(data) > 0 && message.Deserialize(data))
                {
                    std::cout << "[Server] Received: " << message.ToString() << std::endl;
                }
            }
        }
    }
}

void Server::HandleConnection(TcpSocket socket)
{
    SecureChannel channel (socket, HostType::Server);
    _clients.emplace(socket.GetSockfd(), channel);
    std::cout << "[Server] New connection" << std::endl;

    Message message;
    while (true)
    {
        std::vector<uint8_t> data;        
        if (channel.Receive(data) > 0 && message.Deserialize(data))
        {
            std::cout << "[Server] Received: " << message.ToString() << std::endl;
            
            // Send
            for (auto& pair : _clients)
            {
                message.Set("from", "server");
                if (pair.first != socket.GetSockfd())
                    pair.second.Send(message.Serialize());
            }
        }
        else
        {
            break;
        }
    }
    
    _clients.erase(socket.GetSockfd());
    socket.Close();
    std::cout << "[Server] Closed connection" << std::endl;
}