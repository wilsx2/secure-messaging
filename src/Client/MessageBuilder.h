#ifndef MESSAGE_BUILDER_H
#define MESSAGE_BUILDER_H

#include "Network/Message.h"

class MessageBuilder
{
    private:
    static std::vector<std::string> ParseCommandArguments(const std::string& str);
    static Message BuildFromArgs(const std::vector<std::string>& args);
    static Message BuildLogin(const std::vector<std::string>& args);
    static Message BuildRegistration(const std::vector<std::string>& args);
    // static Message BuildListActive(const std::vector<std::string>& args);
    static Message BuildChat(const std::vector<std::string>& args);

    public:
    MessageBuilder() = delete;
    static Message Build(const std::string& command);
};

#endif