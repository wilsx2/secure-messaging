#ifndef A_CRYPT_H
#define A_CRYPT_H

#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>

class AsymCryptHandler
{
    private:
    static CryptoPP::AutoSeededRandomPool _rng;
    CryptoPP::RSA::PrivateKey _rsa_private_key;
    CryptoPP::RSA::PublicKey _rsa_public_key;

    public:
    AsymCryptHandler();
    AsymCryptHandler(std::size_t key_size);

    static std::string Encrypt(const std::string& plaintext, const CryptoPP::RSA::PublicKey& key);
    std::string Decrypt(const std::string& ciphertext);

    const CryptoPP::RSA::PublicKey& GetPublicKey();
    const CryptoPP::RSA::PrivateKey& GetPrivateKey();

    // static std::string KeyToString(const std::array<CryptoPP::byte, KEYLENGTH>& key);
};

#endif