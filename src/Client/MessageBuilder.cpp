#include "Client/MessageBuilder.h"

Message MessageBuilder::Build(const std::string& command)
{
    return BuildFromArgs(ParseCommandArguments(command));
}
std::vector<std::string> MessageBuilder::ParseCommandArguments(const std::string& str)
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

Message MessageBuilder::BuildFromArgs(const std::vector<std::string>& args)
{
    Message message;
    if (args.size() >= 1)
    {
        std::string type = args[0];

             if (type == "login")       return BuildLogin(args);
        else if (type == "register")    return BuildRegistration(args);
        // else if (type == "list")        return BuildListActive(args);
        else if (type == "send")        return BuildChat(args);
    }

    return Message::Error("Command of unrecognized type");
}

Message MessageBuilder::BuildLogin(const std::vector<std::string>& args)
{
    if (args.size() != 3 || args[0] != "login")
        return Message::Error("Command malformed");

    Message message {
        {"type", "login"},
        {"username", args[1]},
        {"password", args[2]}
    };

    return message;
}
Message MessageBuilder::BuildRegistration(const std::vector<std::string>& args)
{
    if (args.size() != 3 || args[0] != "register")
        return Message::Error("Command malformed");

    Message message {
        {"type", "register"},
        {"username", args[1]},
        {"password", args[2]}
    };

    return message;
}
Message MessageBuilder::BuildChat(const std::vector<std::string>& args)
{
    if (args.size() < 3 || args[0] != "send")
        return Message::Error("Command malformed");
    
    Message message {
        {"type", "chat"},
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