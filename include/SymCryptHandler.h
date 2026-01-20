#ifndef S_CRYPT_H
#define S_CRYPT_H

#include <cryptopp/aes.h>
#include <array>

// TODO: Generics for keylength
class SymCryptHandler
{
    private:
    static constexpr std::size_t KEYLENGTH = CryptoPP::AES::DEFAULT_KEYLENGTH;
    std::array<CryptoPP::byte, KEYLENGTH> _key;
    CryptoPP::AES::Encryption _aesEncryption;
    CryptoPP::AES::Decryption _aesDecryption;

    public:
    SymCryptHandler();
    SymCryptHandler(std::array<CryptoPP::byte, KEYLENGTH> key);
    SymCryptHandler(const std::string& secret);

    std::string Encrypt(const std::string& plaintext);
    std::string Decrypt(const std::string& ciphertext);

    const std::array<CryptoPP::byte, KEYLENGTH>& GetKey();
    void SetKey(const std::array<CryptoPP::byte, KEYLENGTH>& key);

    static std::array<CryptoPP::byte, KEYLENGTH> KeyFromSecret(const std::string& secret);
    static std::string KeyToString(const std::array<CryptoPP::byte, KEYLENGTH>& key);
};

#endif