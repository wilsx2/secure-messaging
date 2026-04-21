#pragma once

#include "Client/Client.h"
#include "Shared/Messages.h"
#include <expected>

class ClientCLI
{
    private:
    enum class CommandType { Quit, CheckInbox, ParseFailure };
    using Command = std::variant<Request, CommandType>;
    Client _session;

    std::vector<std::string> ParseCommandArguments(const std::string& str);
    Command BuildCommand(const std::string& command);
    void RunCommand(Command command);

    public:
    ClientCLI() = default;
    ~ClientCLI() = default;
    void Run();
};