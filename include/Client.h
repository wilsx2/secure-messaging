#ifndef CLIENT_H
#define CLIENT_H

#include "SecureChannel.h"
#include "TcpSocket.h"
#include "Message.h"
#include <optional>
#include <memory>
#include <thread>
#include <atomic>

class Client
{
    private:
    SecureChannel _channel;
    std::optional<std::string> _username;
    std::unique_ptr<std::thread> _recv_thread;
    std::unique_ptr<std::thread> _send_thread;
    std::atomic<bool> _running;

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
    // std::optional<Message> BuildRegistrationMessage(const std::vector<std::string>& args);
    // std::optional<Message> BuildListActiveMessage(const std::vector<std::string>& args);
    std::optional<Message> BuildChatMessage(const std::vector<std::string>& args);


    public:
    Client();
    ~Client();
    bool Running();
    void SubmitCommand(const std::string& cmd);
    void SubmitMessage(const Message& msg);
};

#endif