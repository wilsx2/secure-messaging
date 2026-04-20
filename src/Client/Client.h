#pragma once

#include "Network/SecureChannel.h"
#include "Network/TcpSocket.h"
#include "Network/Message.h"
#include "Logging/Logger.h"
#include "Shared/Messages.h"
#include <optional>
#include <expected>
#include <memory>

class Client
{
    private:
    SecureChannel _channel;
    std::vector<uint8_t> _message_buffer;
    std::vector<ReceiveChat> _inbox;
    bool _connected;


    public:
    Client();
    void Connect();
    void Disconnect();
    bool IsConnected();
    std::vector<ReceiveChat>&& GetUnread();
    std::expected<Response, RequestError> SendRequest(Message& request); //TODO: Add timeout
};