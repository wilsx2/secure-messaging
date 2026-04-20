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

std::vector<ReceiveChat>&& Client::GetUnread()
{
    return std::move(_inbox);
}

std::expected<Response, RequestError> Client::SendRequest(Message& request)
{
    Logger::GetInstance().Info("[Client] Sending Request: \"" + request.ToString() + "\"");
    
    if (request.Serialize(_message_buffer))
        if (_channel.Send(_message_buffer))
            while (true)
            {
                if (_channel.Receive(_message_buffer) > 0)
                {
                    ByteReader reader (_message_buffer);
                    uint8_t type_id;
                    reader.Peek(&type_id, sizeof(type_id));
                    if (type_id == Success::TypeId)
                    {
                        Success success;
                        if (success.Deserialize(_message_buffer))
                            return success;
                        else
                            return std::unexpected(RequestError::Deserialization);
                    }
                    else if (type_id == Failure::TypeId)
                    {
                        Failure failure;
                        if (failure.Deserialize(_message_buffer))
                            return failure;
                        else
                            return std::unexpected(RequestError::Deserialization);
                    }
                    else if (type_id == ReceiveChat::TypeId)
                    {
                        ReceiveChat chat;
                        if (chat.Deserialize(_message_buffer))
                            _inbox.push_back(chat);
                        else
                            return std::unexpected(RequestError::Deserialization);
                    }
                    else
                        return std::unexpected(RequestError::Deserialization);
                }
                else
                    return std::unexpected(RequestError::Disconnected);
            }
        else
            return std::unexpected(RequestError::Send);
    else
        return std::unexpected(RequestError::Serialization);
}