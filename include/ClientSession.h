#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include "SecureChannel.h"
#include "TcpSocket.h"
#include "Message.h"
#include <optional>

class ClientSession
{
    private:
    SecureChannel _channel;
    std::optional<std::string> _username;
    bool _connected;

    std::optional<std::string> HandleMessage(const Message& message);
    std::optional<std::string> HandleLoggedInMessage(const Message& message);
    // std::optional<std::string> HandleRegisteredMessage(const Message& message);
    // std::optional<std::string> HandleListMessage(const Message& message);
    std::optional<std::string> HandleChatMessage(const Message& message);
    
    std::vector<std::string> ParseCommandArguments(const std::string& str);
    std::optional<Message> BuildMessage(const std::vector<std::string>& args);
    std::optional<Message> BuildLoginMessage(const std::vector<std::string>& args);
    // std::optional<Message> BuildRegistrationMessage(const std::vector<std::string>& args);
    // std::optional<Message> BuildListActiveMessage(const std::vector<std::string>& args);
    std::optional<Message> BuildChatMessage(const std::vector<std::string>& args);


    public:
    ClientSession();
    void Connect();
    void Disconnect();
    bool Connected();
    void SendCommand(const std::string& cmd);
    void SendRequest(const Message& msg);
    bool TryResponse();
};

#endif