#include "Client/ClientCLI.h"
#include "Client/MessageBuilder.h"
#include <format>

void ClientCLI::Run()
{
    _session.Connect();

    if (_session.Connected())
    {
        _recv_thread = std::make_unique<std::thread>([&](){ReceiveLoop();});
        _send_thread = std::make_unique<std::thread>([&](){SendLoop();});

        while (_session.Connected());

        _recv_thread->join();
        _send_thread->join();
    }
}

void ClientCLI::SendLoop()
{
    std::string input;
    while(_session.Connected())
    {
        std::getline(std::cin, input);

        Message message = MessageBuilder::Build(input);
        if (message.TryGet("type") != "error")
            _session.SendRequest(message);
        else
            PrintMessage(message);
    }
}

void ClientCLI::ReceiveLoop()
{
    while (_session.Connected())
    {
        Message message = _session.AwaitResponse();
        PrintMessage(message);
    }
}

void ClientCLI::PrintMessage(const Message& message)
{
    std::string output = "";

    std::string type = message.Get("type");
    if (type == "error" && message.Has("content"))
        output = message.Get("content");
    else if (type == "registered")
        output = std::format("Registered account \"{}\"", message.Get("username"));
    else if (type == "logged in" && message.Has("username"))
        output = std::format("Logged in as \"{}\"", message.Get("username"));
    else if (type == "chat" && message.HasAll("from", "content"))
        output = std::format("[{}] {}", message.Get("from"), message.Get("content"));
    else if (type == "sent")
        output = "Sent";

    if (output != "") {
        std::cout << output << "\n";
    }
}