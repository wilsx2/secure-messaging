#include "Client/ClientCLI.h"
#include <format>
#include <ranges>
#include <cassert>

void ClientCLI::Run()
{
    _session.Connect();

    std::string input;
    while(_session.IsConnected())
    {
        std::getline(std::cin, input);

        auto command = BuildCommand(input);
        RunCommand(command);
    }
}

void ClientCLI::RunCommand(Command command)
{
    if (command.index() == 0)
    {
        // Send Request
        auto request = std::get<Request>(command);
        auto outcome = std::visit([&](Message& m){ return _session.SendRequest(m); }, request);

        // Print response/error
        if (outcome.has_value())
            std::cout << std::visit([](Message& m){ return m.ToString(); }, outcome.value()) << std::endl;
        else
            std::cout << "Error: " << RequestErrorAsString(outcome.error()) << std::endl;
    }
    else
        // Non-sending command
        switch (std::get<CommandType>(command))
        {
            case CommandType::CheckInbox:   
                for(auto& chat : _session.GetUnread())
                    std::cout << chat.from << "] " << chat.content << std::endl;
                break;
            case CommandType::Quit:
                _session.Disconnect();
                break;
            case CommandType::ParseFailure:
                std::cout << "Command failed to parse" << std::endl;
                break;
        }
}

ClientCLI::Command ClientCLI::BuildCommand(const std::string& command)
{
    auto args = ParseCommandArguments(command);

    if (args.size() > 0)
    {
        if (args[0] == "ping" && args.size() == 1)
            return Ping();
        else if (args[0] == "login" && args.size() == 3)
            return Login(args[1], args[2]);
        else if (args[0] == "register" && args.size() == 3)
            return Register(args[1], args[2]);
        else if (args[0] == "chat" && args.size() >= 3)
        {
            std::string chat = "";
            for (auto c : args | std::views::drop(2) | std::views::join_with(' '))
                chat += c;
            return SendChat(args[1], chat);
        }
        else if (args[0] == "check")
            return CommandType::CheckInbox;
        else if (args[0] == "quit")
            return CommandType::Quit;
    }
    return CommandType::ParseFailure;
}

std::vector<std::string> ClientCLI::ParseCommandArguments(const std::string& str)
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