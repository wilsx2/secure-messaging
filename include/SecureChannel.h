#ifndef SECURE_CHANNEL_H
#define SECURE_CHANNEL_H

#include "TcpSocket.h"
#include <cryptopp/secblock.h>

enum class HostType { Client, Server };
class SecureChannel
{
    private:
    TcpSocket _socket;
    CryptoPP::SecByteBlock _sessionKey;

    public:
    SecureChannel(TcpSocket socket, HostType host_type);
    int Send(const std::string& message);
    int Receive(std::string& message);
};

#endif