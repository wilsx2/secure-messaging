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

TcpSocket::TcpSocket(int sockfd)
{
    // Create wrapper around existing socket
    _sockfd = sockfd;
    TestConnection(_sockfd);

    // Store address
    socklen_t addr_len = sizeof(_addr);
    int success = getsockname(sockfd, (struct sockaddr*)&_addr, &addr_len);
    TestConnection(success);
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
    int address_length = sizeof(_addr);
    int new_sock = accept(_sockfd, (struct sockaddr *)&_addr, (socklen_t *)&address_length);
    return TcpSocket(new_sock);
}
struct sockaddr_in TcpSocket::GetAddr(){
    return _addr;
}
int TcpSocket::GetSockfd(){
    return _sockfd;
}