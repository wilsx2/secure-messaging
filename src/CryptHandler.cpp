#include "CryptHandler.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

CryptHandler::CryptHandler(std::array<byte, KEYLENGTH> key):
    _aesEncryption(_key.data(), KEYLENGTH),
    _aesDecryption(_key.data(), KEYLENGTH)
{
    SetKey(key);
}
CryptHandler::CryptHandler(const std::string& secret): 
    CryptHandler(CryptHandler::KeyFromSecret(secret)) 
{}

const std::array<CryptoPP::byte, CryptHandler::KEYLENGTH>& CryptHandler::GetKey()
{
    return _key;
}

void CryptHandler::SetKey(const std::array<CryptoPP::byte, KEYLENGTH>& key)
{
    memcpy(_key.data(), key.data(), KEYLENGTH);
    _aesEncryption.SetKey(_key.data(), KEYLENGTH);
    _aesDecryption.SetKey(_key.data(), KEYLENGTH);
}

std::array<byte, CryptHandler::KEYLENGTH> CryptHandler::KeyFromSecret(const std::string& secret)
{
    static HKDF<SHA1> hkdf;
    static byte info[] = "secure messaging";
    static size_t ilen = strlen((const char*)info);
    static byte salt[] = "salt";
    static size_t slen = strlen((const char*)salt);

    std::array<byte, KEYLENGTH> key;
    hkdf.DeriveKey(
        key.data(), key.size(), 
        reinterpret_cast<const byte*>(secret.data()), strlen(secret.data()), 
        salt, slen, 
        info, ilen
    );

    return key;
}

std::string CryptHandler::StringFromKey(const std::array<CryptoPP::byte, KEYLENGTH>& key)
{
    std::string result;

    HexEncoder encoder(new StringSink(result));

    encoder.Put(key.data(), key.size());
    encoder.MessageEnd();

    return std::move(result);
}