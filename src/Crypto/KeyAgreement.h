#pragma once

#include "Crypto/SecureKey.h"
#include <cryptopp/secblock.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/integer.h>
#include <cryptopp/oids.h>
#include <cryptopp/osrng.h>
#include <cryptopp/asn.h>
#include <cryptopp/secblock.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <array>
#include <optional>
#include <cstdint>

class KeyAgreement
{
    public:
    KeyAgreement();

    void GetPublicKey(uint8_t*& ptr, std::size_t& length);
    void GetOtherKey(uint8_t*& ptr, std::size_t& length);
    void GetSalt(uint8_t*& ptr, std::size_t& length);
    void ResizeSalt(std::size_t length);
    void GenerateSalt();

    std::optional<SecureKey> Agree();

    private:
    CryptoPP::ECDH<CryptoPP::ECP>::Domain _dh;
    CryptoPP::AutoSeededRandomPool _rng;
    CryptoPP::SecByteBlock _private_key;
    CryptoPP::SecByteBlock _public_key;
    CryptoPP::SecByteBlock _other_key;
    CryptoPP::SecByteBlock _salt;
};