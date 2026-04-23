#pragma once

#include "Network/TcpClient.h"

std::optional<IpAddress> TcpClient::GetRemoteAddress() const
{
    if (_address.ToInteger() == 0)
        return std::nullopt;
    return _address;
}

int TcpClient::GetRemotePort() const
{
    return _port;
}

bool TcpClient::Connect(IpAddress remote_address, int port)
{
    if (_sockfd != -1)
        Close();
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1)
        return false;
    uint32_t addr_int = remote_address.ToInteger();
    if (connect(_sockfd, (struct sockaddr*)&addr_int, sizeof(addr_int)) == -1)
        return false;
        
    _address = remote_address;
    _port = port;
    return true;
}

std::expected<std::size_t, TcpSocket::Error> TcpClient::Send(const void* data, std::size_t size)
{
    std::size_t sent = send(_sockfd, data, size, 0);
    if (sent == -1)
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return std::unexpected(Error::NotReady);
        else if (errno == ENOTSOCK || errno == ENOTCONN || errno == EBADF)
            return std::unexpected(Error::Disconnected);
        else
            return std::unexpected(Error::Unexpected);
    return sent;
}
std::expected<std::size_t, TcpSocket::Error> TcpClient::Receive(void* data, std::size_t size)
{
    std::size_t received = recv(_sockfd, data, size, 0);
    if (received == -1)
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return std::unexpected(Error::NotReady);
        else if (errno == ENOTSOCK || errno == ENOTCONN || errno == EBADF || errno == ECONNREFUSED)
            return std::unexpected(Error::Disconnected);
        else
            return std::unexpected(Error::Unexpected);
    return received;
}