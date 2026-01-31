#include "Client.h"
#include "Global.h"
#include "SecureChannel.h"
#include <thread>

Client::Client()
    : _socket(PORT, INADDR_LOOPBACK)
{ 
    _socket.Connect();
}
Client::~Client()
{
    _socket.Close();
}

void Client::ReceiveLoop(SecureChannel& channel)
{
    std::string message;
    while (true)
    {
        channel.Receive(message);
        
        std::cout << "[Client] Received: " << message << std::endl;
    }
}

void Client::Run()
{
    SecureChannel channel (_socket, HostType::Client);
    std::thread rec ([&](){ReceiveLoop(channel);});

    std::string message;
    while (true)
    {
        std::getline(std::cin, message);
        channel.Send(message);
    }
        
    rec.join();
}