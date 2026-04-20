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
        if (command.index() == 0)
        {
            auto request = std::get<Request>(command);
            std::expected<Response, RequestError>  outcome;
            switch (request.index())
            {
                case 0: outcome = _session.SendRequest(std::get<Ping>(request)); break;
                case 1: outcome = _session.SendRequest(std::get<Login>(request)); break;
                case 2: outcome = _session.SendRequest(std::get<Register>(request)); break;
                case 3: outcome = _session.SendRequest(std::get<SendChat>(request)); break;
            }

            if (outcome.has_value())
            {
                auto response = outcome.value();
                switch (response.index())
                {
                    case 0: std::cout << "Success" << std::endl; break;
                    case 1: std::cout << "Failure: " << std::get<Failure>(response).what << std::endl; break;
                    case 2: assert(false); break;
                }
            }
            else
            {
                std::string error;
                switch (outcome.error())
                {
                    case RequestError::Send:            error = "On send"; break;
                    case RequestError::Disconnected:    error = "Server disconnected"; break;
                    case RequestError::Timeout:         error = "Timeout occurred"; break;
                    case RequestError::Serialization:   error = "Request serialization failed"; break;
                    case RequestError::Deserialization: error = "Response deserialization failed"; break;
                }
                std::cout << "Error: " << error << std::endl;
            }
        }
        else
        {
            auto command_e = std::get<CommandType>(command);
            switch (command_e)
            {
                case CommandType::CheckInbox:   for(auto& chat : _session.GetUnread()) std::cout << chat.from << "] " << chat.content << std::endl; break;
                case CommandType::Quit:         _session.Disconnect(); break;
                case CommandType::ParseFailure: std::cout << "Command failed to parse" << std::endl; break;
            }
        }
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