#include "Global.h"
#include "Server.h"
#include "SecureChannel.h"
#include "Message.h"

Server::Server()
    : _socket(PORT, INADDR_ANY)
{
    _socket.Bind();
    _socket.Listen(1);
}
Server::~Server()
{
    _socket.Close();
}

void Server::Run()
{
    while(true)
    {
        TcpSocket new_connection = _socket.Accept();
        _pool.Enqueue([&](){ HandleConnection(new_connection); });
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