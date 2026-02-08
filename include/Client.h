#ifndef CLIENT_H
#define CLIENT_H

#include "SecureChannel.h"
#include "TcpSocket.h"
#include "Message.h"
#include <optional>

class Client
{
    private:
    SecureChannel _channel;
    std::optional<std::string> _username;
    void ReceiveLoop();
    std::optional<std::string> HandleMessage(const Message& message);
    std::optional<std::string> HandleLoggedInMessage(const Message& message);
    // std::optional<std::string> HandleRegisteredMessage(const Message& message);
    // std::optional<std::string> HandleListMessage(const Message& message);
    std::optional<std::string> HandleChatMessage(const Message& message);
    
    void SendLoop();
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