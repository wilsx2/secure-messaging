#include "AsymCryptHandler.h"

#include <cryptopp/osrng.h>

using namespace CryptoPP;

AsymCryptHandler::AsymCryptHandler(): AsymCryptHandler(2048) {};

AsymCryptHandler::AsymCryptHandler(std::size_t key_size)
{
    static AutoSeededRandomPool rng;
    _rsa_private_key.GenerateRandomWithKeySize(rng, key_size);
    _rsa_public_key = RSA::PublicKey(_rsa_private_key);
}