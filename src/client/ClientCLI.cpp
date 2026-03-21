#include "client/ClientCLI.h"
#include "client/MessageBuilder.h"

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

        if (input.starts_with("exit"))
        {
            _session.Disconnect();
            continue;
        }

        Message message = MessageBuilder::Build(input);
        if (message.TryGet("type") != "error")
            _session.SendRequest(message);
    }
}

void ClientCLI::ReceiveLoop()
{
    while (_session.Connected())
    {
        _session.AwaitResponse();
    }
}