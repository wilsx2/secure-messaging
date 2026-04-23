#pragma once

#include "Network/TcpSocket.h"
#include "Network/IpAddress.h"
#include <expected>
#include <optional>

class TcpListener;
class TcpClient : public TcpSocket
{
    public:
    TcpClient() = default;

    std::optional<IpAddress> GetRemoteAddress() const;
    int GetRemotePort() const;

    bool Connect(IpAddress remote_address, int port);

    std::expected<std::size_t, Error> Send(const void* data, std::size_t size);
    std::expected<std::size_t, Error> Receive(void* data, std::size_t size);

    friend TcpListener;
};
