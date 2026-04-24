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

SecureKey::SecureKey(SecByteBlock&& value) : _value(std::move(value)) { }
SecureKey::SecureKey(SecureKey&& key) : _value (std::move(key._value)) { }

std::size_t SecureKey::Encrypt(void* dest, const void* src, std::size_t dest_blocks, std::size_t src_blocks, uint8_t iv[BlockSize])
{
    std::size_t src_size = src_blocks * BlockSize;
    std::size_t dest_size = dest_blocks * BlockSize;
    AutoSeededRandomPool osrng;
    osrng.GenerateBlock(iv, BlockSize);
    
    AES::Encryption aesEncryption(_value.BytePtr(), _value.SizeInBytes());

    CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);
    ArraySink sink = ArraySink(reinterpret_cast<byte*>(dest), dest_size);
    StreamTransformationFilter stfEncryptor(
        cbcEncryption, 
        new Redirector(sink),
        BlockPaddingSchemeDef::DEFAULT_PADDING
    );
    stfEncryptor.Put(reinterpret_cast<const byte*>(src), src_size);
    stfEncryptor.MessageEnd();

    return sink.TotalPutLength();
}

std::size_t SecureKey::Decrypt(void* dest, const void* src, std::size_t dest_blocks, std::size_t src_blocks, uint8_t iv[BlockSize])
{
    std::size_t src_size = src_blocks * BlockSize;
    std::size_t dest_size = dest_blocks * BlockSize;
    AES::Decryption aesDecryption(_value.BytePtr(), _value.SizeInBytes());

    CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
    ArraySink sink = ArraySink(reinterpret_cast<byte*>(dest), dest_size);
    StreamTransformationFilter stfDecryptor(
        cbcDecryption,
        new Redirector(sink),
        BlockPaddingSchemeDef::DEFAULT_PADDING
    );
    stfDecryptor.Put(reinterpret_cast<const byte*>(src), src_size);
    stfDecryptor.MessageEnd();

    return sink.TotalPutLength();
}