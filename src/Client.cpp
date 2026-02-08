#include "Client.h"
#include "Global.h"
#include "SecureChannel.h"
#include "Message.h"
#include <thread>
#include <format>

Client::Client()
    : _channel(TcpSocket(PORT, INADDR_LOOPBACK))
    , _running(true)
{ 
    _channel.GetSocket().Connect();
    _channel.EstablishKey(HostType::Client);

    _recv_thread = std::make_unique<std::thread>([&](){ReceiveLoop();});
    _send_thread = std::make_unique<std::thread>([&](){SendLoop();});
}
Client::~Client()
{
    if (_recv_thread)
        _recv_thread->join();
    if (_send_thread)
        _send_thread->join();
    _channel.GetSocket().Close();
}

bool Client::Running()
{
    return _running;
}

void Client::ReceiveLoop()
{
    Message message;
    std::vector<uint8_t> data;
    while (_running)
    {
        if (_channel.Receive(data) > 0 && message.Deserialize(data))
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
    std::optional<std::string> type = message.TryGet("type");

         if (type == "logged in")   return HandleLoggedInMessage(message);
    else if (type == "chat")        return HandleChatMessage(message);
    

    return std::nullopt;
}

std::optional<std::string> Client::HandleLoggedInMessage(const Message& message)
{
    if (message.TryGet("type") != "logged in" || !message.Has("username"))
        return std::nullopt;
    
    _username = message.Get("username");    
    return std::format("Logged in successfully as {}", _username.value());
}

std::optional<std::string> Client::HandleChatMessage(const Message& message)
{
    if (message.TryGet("type") != "chat" || message.TryGet("to") != _username || !message.HasAll("from", "content"))
        return std::nullopt;

    const std::string& from = message.Get("from");
    const std::string& content = message.Get("content");
    return std::format("[{}]> {}", from, content);
}

std::vector<std::string> Client::ParseCommandArguments(const std::string& str)
{
    std::vector<std::string> args;

    std::size_t i = 0;
    std::size_t j = str.find(' ', i);
    while (j != std::string::npos)
    {
        args.emplace_back(str.substr(i, j - i));
        i = j + 1;
        j = str.find(' ', i);
    }
    if(i < str.size())
        args.emplace_back(str.substr(i));

    return args;
}

void Client::SubmitCommand(const std::string& cmd)
{
    std::vector<std::string> args = ParseCommandArguments(cmd);
    std::optional<Message> message = BuildMessage(args);
    if (message.has_value())
        SubmitMessage(message.value());
}

void Client::SubmitMessage(const Message& msg)
{
    _channel.Send(msg.Serialize());
}

void Client::SendLoop()
{
    std::string arg;
    std::string input;
    while (_running)
    {
        std::getline(std::cin, input);
        SubmitCommand(input);
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
        else if (type == "exit")        _running = false;
    }

    return std::nullopt;
}

std::optional<Message> Client::BuildLoginMessage(const std::vector<std::string>& args)
{
    if (args.size() != 2 || args[0] != "login")
        return std::nullopt;

    Message message {
        {"type", "login"},
        {"username", args[1]}
    };

    return message;
}
// std::optional<Message> Client::BuildRegistrationMessage(const std::vector<std::string>& args);
// std::optional<Message> Client::BuildListActiveMessage(const std::vector<std::string>& args);
std::optional<Message> Client::BuildChatMessage(const std::vector<std::string>& args)
{
    if (args.size() < 3 || args[0] != "send" || !_username.has_value())
        return std::nullopt;
    
    Message message {
        {"type", "chat"},
        {"from", _username.value()},
        {"to", args[1]},
        {"content", ""}
    };

    std::string content;
    for (std::size_t i = 2; i < args.size(); ++i)
    {
        if (content.size() > 0)
            content += " ";
        content += args[i];
    }
    message.Set("content", content);

    return message;
}