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

    // void Send(const std::string& plaintext);
    // std::string Receive(const std::string& ciphertext);
};

#endif