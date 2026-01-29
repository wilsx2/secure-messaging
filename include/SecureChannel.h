#ifndef SECURE_CHANNEL_H
#define SECURE_CHANNEL_H

#include <cryptopp/secblock.h>

// TODO: Generics for keylength
class SecureChannel
{
    private:
    // TcpSocket& socket_
    // CryptoPP::SecByteBlock _sessionKey;

    public:
    SecureChannel();

    void Send(const std::string& plaintext);
    std::string Receive(const std::string& ciphertext);
};

#endif