#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include "Network/SecureChannel.h"
#include "Network/TcpSocket.h"
#include "Network/Message.h"
#include "Logging/Logger.h"
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