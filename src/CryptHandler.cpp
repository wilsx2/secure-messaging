#include "CryptHandler.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

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

std::string CryptHandler::Encrypt(const std::string& plaintext)
{
    std::string ciphertext;

    // Generate IV
    AutoSeededRandomPool osrng;
    byte iv [AES::BLOCKSIZE];
    osrng.GenerateBlock(iv, AES::BLOCKSIZE); // TODO: Randomly generate IV

    // Prepend IV
    ciphertext.append(reinterpret_cast<const char*>(iv), sizeof(iv));

    // Append Body
    CBC_Mode_ExternalCipher::Encryption cbcEncryption( _aesEncryption, iv );
    StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
    stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length());
    stfEncryptor.MessageEnd();
    
    return ciphertext;
}

std::string CryptHandler::Decrypt(const std::string& ciphertext)
{
    byte iv [AES::BLOCKSIZE];
    memcpy(iv, ciphertext.data(), AES::BLOCKSIZE);

    std::string decrypted;
    CBC_Mode_ExternalCipher::Decryption cbcDecryption(_aesDecryption, iv);
    StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decrypted));
    stfDecryptor.Put(
        reinterpret_cast<const unsigned char*>(ciphertext.data() + AES::BLOCKSIZE),
        ciphertext.size() - AES::BLOCKSIZE
    );
    stfDecryptor.MessageEnd();

    return decrypted;
}

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

std::string CryptHandler::KeyToString(const std::array<CryptoPP::byte, KEYLENGTH>& key)
{
    std::string result;

    HexEncoder encoder(new StringSink(result));

    encoder.Put(key.data(), key.size());
    encoder.MessageEnd();

    return std::move(result);
}