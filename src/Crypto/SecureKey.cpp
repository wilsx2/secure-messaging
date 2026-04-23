#include "Crypto/SecureKey.h"
#include <cryptopp/eccrypto.h>
#include <cryptopp/integer.h>
#include <cryptopp/oids.h>
#include <cryptopp/osrng.h>
#include <cryptopp/asn.h>
#include <cryptopp/secblock.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

SecureKey::SecureKey(SecByteBlock&& value) : _value(value) { }
SecureKey::SecureKey(SecureKey&& key) : _value (std::move(key._value)) { }

void SecureKey::Encrypt(void* dest, const void* src, std::size_t size, uint8_t iv[IV_LENGTH])
{
    AES::Encryption aesEncryption(_value.BytePtr(), _value.SizeInBytes());

    // Generate IV
    AutoSeededRandomPool osrng;
    osrng.GenerateBlock(iv, IV_LENGTH);

    // Encrypt
    CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);
    StreamTransformationFilter stfEncryptor(
        cbcEncryption, 
        new ArraySink(
            reinterpret_cast<byte*>(dest), 
            size
        )
    );
    stfEncryptor.Put(
        reinterpret_cast<const byte*>(src), 
        size
    );
    stfEncryptor.MessageEnd();
}

void SecureKey::Decrypt(void* dest, const void* src, std::size_t size, uint8_t iv[IV_LENGTH])
{
    AES::Decryption aesDecryption (_value.BytePtr(), _value.SizeInBytes());

    // Decrypt
    CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
    StreamTransformationFilter stfDecryptor(
        cbcDecryption, 
        new ArraySink(
            reinterpret_cast<byte*>(dest),
            size
        )
    );
    stfDecryptor.Put(
        reinterpret_cast<const byte*>(src),
        size
    );
    stfDecryptor.MessageEnd();
}