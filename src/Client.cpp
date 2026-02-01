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
    std::vector<uint8_t> data;
    while (true)
    {
        channel.Receive(data);
        
        std::string message (data.begin(), data.end());
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

        std::vector<uint8_t> data (message.begin(), message.end());
        channel.Send(data);
    }
        
    rec.join();
}