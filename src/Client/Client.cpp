#include "Client/Client.h"
#include "Network/SecureChannel.h"
#include "Network/Message.h"
#include "Network/Serialization.h"
#include "Shared/Global.h"
#include "Logging/Logger.h"
#include <format>
#include <expected>

Client::Client()
    : _channel({PORT, INADDR_LOOPBACK})
    , _message_buffer(MESSAGE_BUFFER_SIZE)
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

bool Client::IsConnected()
{
    return _connected;
}

std::vector<ReceiveChat> Client::GetUnread()
{
    std::vector<ReceiveChat> output = std::move(_inbox);
    _inbox.clear();
    return output;
}

std::expected<Response, RequestError> Client::SendRequest(Message& request)
{
    Logger::GetInstance().Info("[Client] Sending Request: \"" + request.ToString() + "\"");
    
    if (request.Serialize(_message_buffer))
        if (_channel.Send(_message_buffer))
            return GetResponse();
        else
            return std::unexpected(RequestError::Send);
    else
        return std::unexpected(RequestError::Serialization);
}

std::expected<Response, RequestError> Client::GetResponse()
{
    if (_channel.Receive(_message_buffer) > 0)
    {
        ByteReader reader (_message_buffer);
        uint8_t type_id;
        reader.Peek(&type_id, sizeof(type_id));
        switch (type_id)
        {
            case Success::TypeId:       return DeserializeAs<Success>();
            case Failure::TypeId:       return DeserializeAs<Failure>();
            case ReceiveChat::TypeId:
            {
                // Our exchange was interrupted by a chat, try to push it onto our inbox and await the real response
                auto chat = DeserializeAs<ReceiveChat>();
                if (chat.has_value())
                {
                    _inbox.push_back(chat.value());
                    return GetResponse();
                }
                return chat;
            }
            default: return std::unexpected(RequestError::Deserialization);
        }
    }
    return std::unexpected(RequestError::Disconnected);
}

template <typename T>
std::expected<T, RequestError> Client::DeserializeAs()
{
    T response;
    if (response.Deserialize(_message_buffer))
        return response;
    return std::unexpected(RequestError::Deserialization);
}
template std::expected<Success, RequestError> Client::DeserializeAs<Success>();
template std::expected<Failure, RequestError> Client::DeserializeAs<Failure>();
template std::expected<ReceiveChat, RequestError> Client::DeserializeAs<ReceiveChat>();