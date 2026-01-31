#ifndef SECURE_CHANNEL_H
#define SECURE_CHANNEL_H

#include "TcpSocket.h"
#include <cryptopp/secblock.h>

enum class HostType { Client, Server };
class SecureChannel
{
    private:
    TcpSocket _socket;
    CryptoPP::SecByteBlock _session_key;

    public:
    SecureChannel(TcpSocket socket, HostType host_type);
    SecureChannel(const SecureChannel& other);
    int Send(const std::string& message);
    int Receive(std::string& message);
    TcpSocket GetSocket();
};

#endif