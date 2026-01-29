#include "TcpSocket.h"
#include <vector>

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
TcpSocket::~TcpSocket()
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

void TcpSocket::Write(std::string message)
{
    int success = write(_sockfd, (const void*) message.data(), message.size());
    CheckSyscall(success, "Failed to write to socket");
}

std::string TcpSocket::Read(std::size_t buffer_size)
{
    std::vector<char> buff(buffer_size + 1);
    int bytes = read(_sockfd, buff.data(), buffer_size);

    CheckSyscall(bytes, "Failed to read from socket");
    if (bytes <= 0)
        return "";

    buff[bytes] = '\0';
    return std::string(buff.data());
}


struct sockaddr_in TcpSocket::GetAddr(){
    return _addr;
}
int TcpSocket::GetSockfd(){
    return _sockfd;
}