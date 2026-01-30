#include "Global.h"
#include "Server.h"
#include "SecureChannel.h"

Server::Server()
    : _socket(PORT, INADDR_ANY)
{ }

void Server::Run()
{
    while(true)
    {
        TcpSocket new_connection = _socket.Accept();
        if (_threads.size() < 16)
            _threads.emplace_back([&](){HandleConnection(new_connection);});
        else
            new_connection.Close();
    }
}

void Server::HandleConnection(TcpSocket socket)
{
    SecureChannel channel (socket, HostType::Server);

    std::string message = channel.Receive();
    std::cout << "[Server] Received: " << message << std::endl;
    channel.Send("Hello from server");
    
    socket.Close();
}