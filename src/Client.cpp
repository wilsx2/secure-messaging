#include "Client.h"
#include "Global.h"
#include "SecureChannel.h"

Client::Client()
    : _socket(PORT, INADDR_LOOPBACK)
{ 
    _socket.Connect();
}
Client::~Client()
{
    _socket.Close();
}

void Client::Run()
{
    SecureChannel channel (_socket, HostType::Client);

    while (true)
    {
        std::string output;
        std::cin >> output;
        channel.Send(output);
        std::string message;
        channel.Receive(message);
        std::cout << "[Client] Received: " << message << std::endl;
    }
}