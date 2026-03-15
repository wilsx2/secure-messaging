#ifndef SECURE_CHANNEL_H
#define SECURE_CHANNEL_H

#include "TcpSocket.h"
#include <cryptopp/secblock.h>
#include <vector>
#include <cstdint>

enum class HostType { Client, Server };
class SecureChannel
{
    private:
    static constexpr std::size_t SALT_LEN = 256;
    TcpSocket _socket;
    CryptoPP::SecByteBlock _session_key;

    public:
    SecureChannel(TcpSocket&& socket);
    SecureChannel(SecureChannel&& other);
    bool EstablishKey(HostType host_type);
    int Send(const std::vector<uint8_t>& message);
    int Receive(std::vector<uint8_t>& message);
    const TcpSocket& GetSocket();
};

#endif