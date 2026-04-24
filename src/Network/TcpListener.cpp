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
    int opt = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(remote_address.ToInteger());
    if (bind(_sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
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
    TcpClient socket (handle);
    return socket;
}