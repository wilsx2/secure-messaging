#include "Network/TcpListener.h"

std::optional<IpAddress> TcpListener::GetLocalAddress() const
{
    return _address;
}
int TcpListener::GetLocalPort() const
{
    return _port;
}

bool TcpListener::Listen(IpAddress remote_address, int port)
{
    if (_sockfd == -1)
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1)
        return false;
    uint32_t addr_int = remote_address.ToInteger();
    if (bind(_sockfd, (struct sockaddr*)&addr_int, sizeof(addr_int)) == -1)
        return false;
    if (listen(_sockfd, 128) == -1)
        return false;

    _address = remote_address;
    _port = port;
    return true;
}
std::expected<TcpClient, TcpSocket::Error> TcpListener::Accept()
{
    TcpSocket::Handle handle = accept(_sockfd, nullptr, nullptr);
    if (handle == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return std::unexpected(Error::NotReady);
        else if (errno == ENOTSOCK || errno == EINVAL)
            return std::unexpected(Error::Disconnected);
        else
            return std::unexpected(Error::Unexpected);
    }
    TcpClient socket;
    socket._sockfd = handle;
    return socket;
}