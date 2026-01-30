#include "SecureChannel.h"

#include <iostream>
#include <stdexcept>
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

SecureChannel::SecureChannel(TcpSocket socket, HostType host_type)
    : _socket(socket)
    , _sessionKey(AES::DEFAULT_KEYLENGTH)
{
    // Key Agreement
    OID CURVE = ASN1::secp256r1();
    AutoSeededRandomPool rng;
    ECDH < ECP >::Domain dh( CURVE );
    SecByteBlock priv(dh.PrivateKeyLength()), pub(dh.PublicKeyLength()), pubOther(dh.PublicKeyLength());
    dh.GenerateKeyPair(rng, priv, pub);
    SecByteBlock shared(dh.AgreedValueLength());

    if (host_type == HostType::Client)
    {
        _socket.Write(pub.BytePtr(), pub.SizeInBytes());
        _socket.Read(pubOther.BytePtr(), pubOther.SizeInBytes());
    }
    else // host_type == HostType::Server
    {
        _socket.Read(pubOther.BytePtr(), pubOther.SizeInBytes());
        _socket.Write(pub.BytePtr(), pub.SizeInBytes());
    }
    
    if(!dh.Agree(shared, priv, pubOther))
        throw std::runtime_error("Failed to reach shared secret");

    
    // Key Derivation
    HKDF<SHA1> hkdf;
    byte info[] = "secure messaging";
    byte salt[] = "salt";
    hkdf.DeriveKey(
        _sessionKey.BytePtr(), _sessionKey.SizeInBytes(), 
        shared.BytePtr(), shared.SizeInBytes(),
        salt, strlen((const char*)salt), 
        info, strlen((const char*)info)
    );
}

int SecureChannel::Send(const std::string& message)
{
    AES::Encryption aesEncryption(_sessionKey.BytePtr(), _sessionKey.SizeInBytes());
    std::string ciphertext;

    // Generate IV
    AutoSeededRandomPool osrng;
    byte iv [AES::BLOCKSIZE];
    osrng.GenerateBlock(iv, AES::BLOCKSIZE);

    // Prepend IV
    ciphertext.append(reinterpret_cast<const char*>(iv), sizeof(iv));

    // Append Body
    CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );
    StreamTransformationFilter stfEncryptor(cbcEncryption, new StringSink(ciphertext));
    stfEncryptor.Put(reinterpret_cast<const unsigned char*>(message.c_str()), message.length());
    stfEncryptor.MessageEnd();

    // Write encrypted message to socket
    return _socket.Write(ciphertext.data(), ciphertext.size());
}

int SecureChannel::Receive(std::string& message)
{
    std::string output = "";

    // Read encrypted message from socket
    char ciphertext [1024]; // TODO: Write and read message size
    std::size_t ciphertext_size = _socket.Read(ciphertext, sizeof(ciphertext));
    if (ciphertext_size > 0)
    {
        // Decrypt Ciphertext
        AES::Decryption aesDecryption (_sessionKey.BytePtr(), _sessionKey.SizeInBytes());

        /// Copy IV
        byte iv [AES::BLOCKSIZE];
        memcpy(iv, ciphertext, AES::BLOCKSIZE);

        /// Decrypt body
        CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
        StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(output));
        stfDecryptor.Put(
            reinterpret_cast<const unsigned char*>(ciphertext + AES::BLOCKSIZE),
            ciphertext_size - AES::BLOCKSIZE
        );
        stfDecryptor.MessageEnd();
    }

    message = output;
    return ciphertext_size;
}