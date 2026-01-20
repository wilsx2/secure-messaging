#ifndef A_CRYPT_H
#define A_CRYPT_H

#include <cryptopp/rsa.h>

class AsymCryptHandler
{
    private:
    CryptoPP::RSA::PrivateKey _rsa_private_key;
    CryptoPP::RSA::PublicKey _rsa_public_key;

    public:
    AsymCryptHandler();
    AsymCryptHandler(std::size_t key_size);

    std::string Decrypt(const std::string& ciphertext);

    const CryptoPP::RSA::PublicKey& GetPublicKey();
    const CryptoPP::RSA::PrivateKey& GetPrivateKey();

    static std::string Encrypt(const std::string& plaintext, const CryptoPP::RSA::PublicKey& key);
    // static std::string KeyToString(const std::array<CryptoPP::byte, KEYLENGTH>& key);
};

#endif