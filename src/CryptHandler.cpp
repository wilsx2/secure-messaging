#include "CryptHandler.h"


CryptHandler::CryptHandler(std::array<CryptoPP::byte, KEYLENGTH> key):
    _aesEncryption(_key.data(), KEYLENGTH),
    _aesDecryption(_key.data(), KEYLENGTH)
{
    memcpy(_key.data(), key.data(), KEYLENGTH);
    _aesEncryption.SetKey(_key.data(), KEYLENGTH);
    _aesDecryption.SetKey(_key.data(), KEYLENGTH);
}