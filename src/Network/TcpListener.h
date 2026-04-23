#pragma once

#include "Network/TcpSocket.h"
#include "Network/TcpClient.h"
#include "Network/IpAddress.h"
#include <expected>
#include <optional>

class TcpListener : public TcpSocket
{
    public:
    TcpListener() = default;

    std::optional<IpAddress> GetLocalAddress() const;
    int GetLocalPort() const;

    bool Listen(IpAddress remote_address, int port);

    std::expected<TcpClient, Error> Accept();
};