#include "AsymCryptHandler.h"

using namespace CryptoPP;

AutoSeededRandomPool AsymCryptHandler::_rng;

AsymCryptHandler::AsymCryptHandler(): AsymCryptHandler(2048) {};

AsymCryptHandler::AsymCryptHandler(std::size_t key_size)
{
    _rsa_private_key.GenerateRandomWithKeySize(_rng, key_size);
    _rsa_public_key = RSA::PublicKey(_rsa_private_key);
}

std::string AsymCryptHandler::Encrypt(const std::string& plaintext, const RSA::PublicKey& key)
{
    std::string ciphertext;

    RSAES_OAEP_SHA_Encryptor e(key);

    StringSource ss1(plaintext, true,
        new PK_EncryptorFilter(_rng, e,
            new StringSink(ciphertext)
        )
    );

    return ciphertext;
}

std::string AsymCryptHandler::Decrypt(const std::string& ciphertext)
{
    std::string plaintext;

    RSAES_OAEP_SHA_Decryptor e(_rsa_private_key);

    StringSource ss1(ciphertext, true,
        new PK_DecryptorFilter(_rng, e,
            new StringSink(plaintext)
        )
    );

    return plaintext;
}

const RSA::PublicKey& AsymCryptHandler::GetPublicKey()
{
    return _rsa_public_key;
}
const RSA::PrivateKey& AsymCryptHandler::GetPrivateKey()
{
    return _rsa_private_key;
}