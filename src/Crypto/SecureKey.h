#pragma once
#include <cryptopp/secblock.h>
#include <cryptopp/aes.h>
#include <array>

class KeyAgreement;
class SecureKey
{
    public:
    static constexpr std::size_t BlockSize = CryptoPP::AES::BLOCKSIZE;
    
    SecureKey(SecureKey&& key);
    void Encrypt(void* dest, const void* src, std::size_t blocks, uint8_t iv[BlockSize]);
    void Decrypt(void* dest, const void* src, std::size_t blocks, uint8_t iv[BlockSize]);

    private:
    CryptoPP::SecByteBlock _value;
    SecureKey(CryptoPP::SecByteBlock&& value);

    friend KeyAgreement;
};