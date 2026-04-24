#include "Network/TcpSocket.h"
#include <fcntl.h>
#include <vector>
#include <cstring>

TcpSocket::TcpSocket()
    : _sockfd(-1)
    , _address(0)
    , _port(0)
{ }

TcpSocket::TcpSocket(TcpSocket&& socket)
    : _sockfd(socket._sockfd)
    , _address(socket._address)
    , _port(socket._port)
{
    socket._sockfd = -1;
    socket._address = IpAddress(0);
    socket._port = 0;
}

TcpSocket::~TcpSocket()
{
    Close();
}

void TcpSocket::Close()
{
    if (_sockfd != -1)
    {
        close(_sockfd);
        _sockfd = -1;
    }
    _address = IpAddress(0);
    _port = 0;
}

void TcpSocket::SetBlocking(bool blocking)
{
    int flags = fcntl(_sockfd, F_GETFL, 0);
    if (flags != -1) fcntl(_sockfd, F_SETFL, blocking ? (flags & (~O_NONBLOCK)) : (flags | O_NONBLOCK));
}
bool TcpSocket::IsBlocking() const
{
    int flags = fcntl(_sockfd, F_GETFL, 0);
    return !(flags & O_NONBLOCK);
}

TcpSocket::Handle TcpSocket::GetHandle() const
{
    return _sockfd;
}