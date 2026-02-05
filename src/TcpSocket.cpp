#include "TcpSocket.h"
#include <fcntl.h>
#include <vector>
#include <cstring>

TcpSocket::TcpSocket(int port, u_long interface)
{
    // Define address structure
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = htonl(interface);

    // Establish socket
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    CheckSyscall(_sockfd, "Failed to create socket");

    int opt = 1;
    int result = setsockopt(
        _sockfd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );
    CheckSyscall(_sockfd, "Failed to set sockopt");
}
TcpSocket::TcpSocket(int sockfd, struct sockaddr_in addr)
{
    // Create wrapper around existing socket
    _sockfd = sockfd;
    _addr = addr;
    CheckSyscall(_sockfd, "Failed to create wrapper around sockfd " + std::to_string(sockfd));
}
TcpSocket::TcpSocket(int sockfd)
{
    // Create wrapper around existing socket
    _sockfd = sockfd;
    socklen_t addr_len = sizeof(_addr);
    getsockname(sockfd, (sockaddr*)&_addr, &addr_len);
    CheckSyscall(_sockfd, "Failed to create wrapper around sockfd " + std::to_string(sockfd));
}

void TcpSocket::Close()
{
    close(_sockfd);
}

void TcpSocket::CheckSyscall(int result, std::string error_message)
{
    if(result < 0){
        error_message = "ERROR: " + error_message;
        perror(error_message.c_str());
    }
}

int TcpSocket::SetNonBlocking()
{
    int flags = fcntl(_sockfd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(_sockfd, F_SETFL, flags | O_NONBLOCK);
}

int TcpSocket::Bind()
{
    int result = bind(_sockfd, (struct sockaddr*)&_addr, sizeof(_addr));
    CheckSyscall(result, "Failed to bind socket");
    return result;
}
int TcpSocket::Connect()
{
    int result = connect(_sockfd, (struct sockaddr*)&_addr, sizeof(_addr));
    CheckSyscall(result, "Failed to connect socket");
    return result;
}
int TcpSocket::Listen(int backlog)
{
    int result = listen(_sockfd, backlog);
    CheckSyscall(result, "Failed to listen from socket");
    return result;
}
TcpSocket TcpSocket::Accept()
{
    struct sockaddr_in addr;
    int address_length = sizeof(addr);
    int new_sockfd = accept(_sockfd, (struct sockaddr *)&addr, (socklen_t *)&address_length);
    return TcpSocket(new_sockfd, addr);
}

int TcpSocket::Send(const void* data, std::size_t size, int flags)
{
    int success = send(_sockfd, data, size, flags);
    CheckSyscall(success, "Failed to write to socket");
    return success;
}

int TcpSocket::SendBytes(const std::vector<uint8_t>& bytes)
{
    std::size_t message_len = bytes.size();
    if (Send(&message_len, sizeof(message_len), 0) < 0)
        return -1;
    return Send(bytes.data(), message_len, 0);
}

int TcpSocket::Receive(void* data, std::size_t size, int flags)
{
    int bytes = recv(_sockfd, data, size, flags);
    CheckSyscall(bytes, "Failed to read from socket");
    return bytes;
}

int TcpSocket::ReceiveAll(void* data, std::size_t size)
{
    int total_received = 0;

    while (total_received < size)
    {
        std::size_t last_received = Receive(
            reinterpret_cast<uint8_t*>(data) + total_received,
            size - total_received,
            0
        );
        if (last_received <= 0)
            return -1;
        total_received += last_received;
    }

    return total_received;
}

int TcpSocket::ReceiveBytes(std::vector<uint8_t>& bytes)
{
    std::vector<uint8_t> message;
    std::size_t message_len;

    if(ReceiveAll(&message_len, sizeof(message_len)) == -1)
        return -1;

    message.resize(message_len);
    if (ReceiveAll(message.data(), message_len) == -1)
        return -1;
    
    bytes = std::move(message);
    return message_len;
}

struct sockaddr_in TcpSocket::GetAddr(){
    return _addr;
}
int TcpSocket::GetFd(){
    return _sockfd;
}