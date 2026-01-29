#include "TcpSocket.h"

TcpSocket::TcpSocket(int port, u_long interface)
{
    // Define address structure
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = htonl(interface);

    // Establish socket
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    TestConnection(_sockfd);
}

TcpSocket::TcpSocket(int sockfd, struct sockaddr_in addr)
{
    // Create wrapper around existing socket
    _sockfd = sockfd;
    _addr = addr;
    TestConnection(_sockfd);
}

TcpSocket::~TcpSocket(){
    close(_sockfd);
}

void TcpSocket::TestConnection(int item){
    if(item < 0){
        perror("Failed to connect");
        exit(EXIT_FAILURE);
    }
}

int TcpSocket::Bind(){
    int result = bind(_sockfd, (struct sockaddr*)&_addr, sizeof(_addr));
    TestConnection(result);
    return result;
}
int TcpSocket::Connect(){
    int result = connect(_sockfd, (struct sockaddr*)&_addr, sizeof(_addr));
    TestConnection(result);
    return result;
}
int TcpSocket::Listen(int backlog){
    int result = listen(_sockfd, backlog);
    TestConnection(result);
    return result;
}
TcpSocket TcpSocket::Accept(){
    struct sockaddr_in addr;
    int address_length = sizeof(addr);
    int new_sockfd = accept(_sockfd, (struct sockaddr *)&_addr, (socklen_t *)&address_length);
    return TcpSocket(new_sockfd, addr);
}

void TcpSocket::Write(std::string message){
    int success = write(_sockfd, (const void*) message.data(), message.size());
    TestConnection(success);
}

std::string TcpSocket::Read(std::size_t buffer_size){
    char buff[buffer_size];
    int bytes = read(_sockfd, buff, buffer_size);
    buff[bytes] = '\n'; // Add null termination
    return std::string(buff);
}

struct sockaddr_in TcpSocket::GetAddr(){
    return _addr;
}
int TcpSocket::GetSockfd(){
    return _sockfd;
}