#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include "network/SecureChannel.h"
#include "network/TcpSocket.h"
#include "network/Message.h"
#include "logging/Logger.h"
#include <optional>
#include <memory>

class Client
{
    private:
    SecureChannel _channel;
    std::string _username;
    bool _authenticated;
    bool _connected;

    void HandleResponse(const Message& message);

    public:
    Client();
    void Connect();
    void Disconnect();
    bool Connected();
    void SendRequest(const Message& msg);
    Message AwaitResponse();
    // TODO: std::optional<Message> TryResponse();
};

#endif