#include "Client.h"
#include "Global.h"
#include "SecureChannel.h"
#include "Message.h"
#include <thread>
#include <format>

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
            std::optional<std::string> output = HandleMessage(message);
            if (output.has_value())
            {

                std::cout << output.value() << std::endl;
            }
        }
    }
}

std::optional<std::string> Client::HandleMessage(const Message& message)
{
    std::string type = message.Get("type").value_or("");

    if (type == "send") return HandleChatMessage(message);

    return std::nullopt;
}

std::optional<std::string> Client::HandleChatMessage(const Message& message)
{
    if (!message.Has("type") || !message.Has("to") || !message.Has("from") || !message.Has("content"))
        return std::nullopt;

    std::string type, to, from, content;
    type = message.Get("type").value();
    to = message.Get("to").value();
    from = message.Get("from").value();
    content = message.Get("content").value();
    if (type != "send"/*  || to != me */)
        return std::nullopt;
    
    return std::format("[{}]> {}", from, content);

}

void Client::SendLoop(SecureChannel& channel)
{
    std::string arg;
    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        std::vector<std::string> args = ParseCommandArguments(input);
        std::optional<Message> message = BuildMessage(args);
        if (message.has_value())
            channel.Send(message.value().Serialize());
    }
}

std::optional<Message> Client::BuildMessage(const std::vector<std::string>& args)
{
    Message message;
    if (args.size() >= 1)
    {
        std::string type = args[0];

             if (type == "login")       return BuildLoginMessage(args);
        // else if (type == "register")    return BuildRegistrationMessage(args);
        // else if (type == "list")        return BuildListActiveMessage(args);
        else if (type == "send")        return BuildChatMessage(args);
    }

    return message;
}

std::optional<Message> Client::BuildLoginMessage(const std::vector<std::string>& args)
{
    if (args.size() != 2 || args[0] != "login")
        return std::nullopt;

    Message message;
    message.Set("type", "login");
    message.Set("username", args[1]);

    return message;
}
// std::optional<Message> Client::BuildRegistrationMessage(const std::vector<std::string>& args);
// std::optional<Message> Client::BuildListActiveMessage(const std::vector<std::string>& args);
std::optional<Message> Client::BuildChatMessage(const std::vector<std::string>& args)
{
    if (args.size() < 3 || args[0] != "send")
        return std::nullopt;
    
    Message message;
    message.Set("type", "send");
    message.Set("to", args[1]);

    std::string content;
    for (int i = 2; i < args.size(); ++i)
    {
        if (content.size() > 0)
            content += " ";
        content += args[i];
    }
    message.Set("content", content);
    return message;
}

void Client::Run()
{
    SecureChannel channel (_socket, HostType::Client);
    std::thread rec ([&](){ReceiveLoop(channel);});
    SendLoop(channel);        
    rec.join();
}