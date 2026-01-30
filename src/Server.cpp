#include "Global.h"
#include "Server.h"
#include "SecureChannel.h"

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
        HandleConnection(new_connection);
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