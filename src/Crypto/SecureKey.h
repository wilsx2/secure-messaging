#pragma once
#include <cryptopp/secblock.h>
#include <cryptopp/rijndael.h>
#include <array>

class KeyAgreement;
class SecureKey
{
    public:
    static constexpr std::size_t IV_LENGTH = CryptoPP::AES::BLOCKSIZE;
    
    SecureKey(SecureKey&& key);
    void Encrypt(void* dest, const void* src, std::size_t length, uint8_t iv[IV_LENGTH]);
    void Decrypt(void* dest, const void* src, std::size_t length, uint8_t iv[IV_LENGTH]);

    private:
    CryptoPP::SecByteBlock _value;
    SecureKey(CryptoPP::SecByteBlock&& value);

    friend KeyAgreement;
};