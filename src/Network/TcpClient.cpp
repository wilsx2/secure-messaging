#include "Network/TcpClient.h"

TcpClient::TcpClient(int handle)
    : TcpSocket()
{
    _sockfd = handle;
}

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

bool TcpClient::IsConnected() const
{
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(_sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
    return retval == 0 && error == 0;
}

bool TcpClient::Connect(IpAddress remote_address, int port)
{
    if (_sockfd != -1)
        Close();
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1)
        return false;
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(remote_address.ToInteger());
    if (connect(_sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        return false;

    _address = remote_address;
    _port = port;
    return true;
}

std::expected<std::size_t, TcpSocket::Error> TcpClient::Send(const void* data, std::size_t size)
{
    ssize_t sent = send(_sockfd, data, size, 0);
    if (sent == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return std::unexpected(Error::NotReady);
        else if (errno == ENOTSOCK || errno == ENOTCONN || errno == EBADF)
            return std::unexpected(Error::Disconnected);
        else
            return std::unexpected(Error::Unexpected);
    }
    return static_cast<std::size_t>(sent);
}

std::expected<std::size_t, TcpSocket::Error> TcpClient::Receive(void* data, std::size_t size)
{
    ssize_t received = recv(_sockfd, data, size, 0);
    if (received == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return std::unexpected(Error::NotReady);
        else if (errno == ENOTSOCK || errno == ENOTCONN || errno == EBADF || errno == ECONNREFUSED)
            return std::unexpected(Error::Disconnected);
        else
            return std::unexpected(Error::Unexpected);
    }
    return static_cast<std::size_t>(received);
}