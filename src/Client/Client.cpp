#include "Client/Client.h"
#include "Network/SecureChannel.h"
#include "Network/Message.h"
#include "Shared/Global.h"
#include "Logging/Logger.h"
#include <format>

Client::Client()
    : _channel({PORT, INADDR_LOOPBACK})
    , _authenticated(false)
    , _connected(false)
{ }

void Client::Connect()
{
    if (_channel.EstablishKey(HostType::Client))
    {
        _connected = true;
    }
}

void Client::Disconnect()
{
    // TODO: Disconnect socket
    _connected = false;
}

bool Client::Connected()
{
    return _connected;
}

void Client::SendRequest(const Message& msg)
{
    if (_channel.Send(msg.Serialize()))
        Logger::GetInstance().Info("[Client] Sent request: \"" + msg.ToString() + "\"");
    else
        Logger::GetInstance().Error("[Client] Failed to send request: \"" + msg.ToString() + "\"");
}

Message Client::AwaitResponse()
{
    Message message;
    std::vector<uint8_t> data;
    if (_channel.Receive(data) > 0 && message.Deserialize(data))
    {
        Logger::GetInstance().Info("[Client] Received response: \"" + message.ToString() + "\"");
        HandleResponse(message);
    }
    else
    {
        Disconnect();
        message = Message::Error("Response malformed");
    }
    
    return message;
}

void Client::HandleResponse(const Message& message)
{
    std::optional<std::string> type = message.TryGet("type");
    if (message.TryGet("type") == "logged in" && message.Has("username"))
    {
        _username = message.Get("username");
    }
}