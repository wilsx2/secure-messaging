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

std::vector<std::string> Client::ParseCommandArguments(const std::string& str)
{
    std::vector<std::string> args;

    std::size_t i = 0;
    std::size_t j = str.find(' ', i);
    while (j != -1)
    {
        args.emplace_back(str.substr(i, j - i));
        i = j + 1;
        j = str.find(' ', i);
    }
    if(i < str.size())
        args.emplace_back(str.substr(i));

    return args;
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
    message.Set("type", "echo");
    std::string arg;
    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        std::vector<std::string> args = ParseCommandArguments(input);

        if (args.size() >= 1)
        {
            std::string content;

            for (int i = 0; i < args.size(); ++i)
            {
                if (content.size() > 0)
                    content += " ";
                content += args[i];
            }
            message.Set("content", content);

            channel.Send(message.Serialize());
        }
    }
        
    rec.join();
}