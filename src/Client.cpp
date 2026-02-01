#include "Client.h"
#include "Global.h"
#include "SecureChannel.h"
#include "Message.h"
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
    Message message;
    std::vector<uint8_t> data;
    while (true)
    {
        if (channel.Receive(data) > 0 && message.Deserialize(data))
        {
            std::cout << "[Client] Received: " << message.ToString() << std::endl;
        }
    }
}

void Client::Run()
{
    SecureChannel channel (_socket, HostType::Client);
    std::thread rec ([&](){ReceiveLoop(channel);});

    Message message;
    message.Set("type", "chat");
    message.Set("from", "client");
    std::string content;
    while (true)
    {
        std::getline(std::cin, content);
        message.Set("content", content);
        channel.Send(message.Serialize());
    }
        
    rec.join();
}