#pragma once

#include "Crypto/SecureKey.h"
#include <cryptopp/secblock.h>
#include <array>
#include <cstdint>

class KeyAgreement
{
    public:
    KeyAgreement() = default;

    void GetPublicKey(uint8_t*& ptr, std::size_t& length);
    void GetOtherKey(uint8_t*& ptr, std::size_t& length);
    void GetSalt(uint8_t*& ptr, std::size_t& length);
    void GenerateSalt(std::size_t length);

    std::optional<SecureKey> Agree();

    private:
    ECDH<ECP>::Domain _dh;
    AutoSeededRandomPool _rng;
    CryptoPP::SecByteBlock _private_key;
    CryptoPP::SecByteBlock _public_key;
    CryptoPP::SecByteBlock _other_key;
    CryptoPP::SecByteBlock _salt;
};