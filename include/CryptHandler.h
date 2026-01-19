#ifndef CRYPT_H
#define CRYPT_H

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <array>

// TODO: Generics for keylength
class CryptHandler
{
    private:
    static constexpr std::size_t KEYLENGTH = CryptoPP::AES::DEFAULT_KEYLENGTH;
    std::array<CryptoPP::byte, KEYLENGTH> _key;
    CryptoPP::AES::Encryption _aesEncryption;
    CryptoPP::AES::Decryption _aesDecryption;

    public:
    CryptHandler(std::array<CryptoPP::byte, KEYLENGTH> key);

    std::string Encrypt(const std::string& plaintext);
    std::string Decrypt(const std::string& ciphertext);

    void SetKey(std::array<CryptoPP::byte, KEYLENGTH> key);

    static std::array<CryptoPP::byte, KEYLENGTH> KeyFromString(const std::string& string); // Use KDF
}

#endif