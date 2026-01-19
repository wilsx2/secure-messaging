#ifndef CRYPT_H
#define CRYPT_H

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>

// TODO: Generics for
class CryptHandler
{
    private:
    CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
    CryptoPP::AES::Encryption aesEncryption;
    CryptoPP::AES::Decryption aesDecryption;

    public:
    CryptHandler(std::string key)
}

#endif