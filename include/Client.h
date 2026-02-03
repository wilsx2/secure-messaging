#ifndef CLIENT_H
#define CLIENT_H

#include "SecureChannel.h"
#include "TcpSocket.h"
#include "Message.h"

class Client
{
    private:
    TcpSocket _socket;
    void ReceiveLoop(SecureChannel& channel);
    std::vector<std::string> ParseCommandArguments(const std::string& str);
    
    std::optional<Message> BuildMessage(const std::vector<std::string>& args);
    std::optional<Message> BuildLoginMessage(const std::vector<std::string>& args);
    std::optional<Message> BuildRegistrationMessage(const std::vector<std::string>& args);
    std::optional<Message> BuildListActiveMessage(const std::vector<std::string>& args);
    std::optional<Message> BuildChatMessage(const std::vector<std::string>& args);


    public:
    Client();
    ~Client();
    void Run();
};

#endif