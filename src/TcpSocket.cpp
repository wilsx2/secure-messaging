#include "TcpSocket.h"

TcpSocket::TcpSocket(int port, u_long interface)
{
    // Define address structure
    m_address.sin_family = AF_INET;
    m_address.sin_port = htons(port);
    m_address.sin_addr.s_addr = htonl(interface);

    // Establish socket
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    TestConnection(m_sock);
}

TcpSocket::~TcpSocket(){
    close(m_sock);
}

void TcpSocket::TestConnection(int item){
    if(item < 0){
        perror("Failed to connect");
        exit(EXIT_FAILURE);
    }
}

int TcpSocket::Bind(){
    int result = bind(m_sock, (struct sockaddr*)&m_address, sizeof(m_address));
    TestConnection(result);
    return result;
}
int TcpSocket::Connect(){
    int result = connect(m_sock, (struct sockaddr*)&m_address, sizeof(m_address));
    TestConnection(result);
    return result;
}
int TcpSocket::Listen(int backlog){
    int result = listen(m_sock, backlog);
    TestConnection(result);
    return result;
}
struct sockaddr_in TcpSocket::GetAddress(){
    return m_address;
}
int TcpSocket::GetSock(){
    return m_sock;
}