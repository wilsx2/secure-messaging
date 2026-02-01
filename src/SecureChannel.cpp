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
    , _session_key(AES::DEFAULT_KEYLENGTH)
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
        _socket.Send(pub.BytePtr(), pub.SizeInBytes(), MSG_WAITALL);
        _socket.Receive(pubOther.BytePtr(), pubOther.SizeInBytes(), MSG_WAITALL);
    }
    else // host_type == HostType::Server
    {
        _socket.Receive(pubOther.BytePtr(), pubOther.SizeInBytes(), MSG_WAITALL);
        _socket.Send(pub.BytePtr(), pub.SizeInBytes(), MSG_WAITALL);
    }
    
    if(!dh.Agree(shared, priv, pubOther))
        throw std::runtime_error("Failed to reach shared secret");

    
    // Key Derivation
    HKDF<SHA1> hkdf;
    byte info[] = "secure messaging";
    byte salt[] = "salt";
    hkdf.DeriveKey(
        _session_key.BytePtr(), _session_key.SizeInBytes(), 
        shared.BytePtr(), shared.SizeInBytes(),
        salt, strlen((const char*)salt), 
        info, strlen((const char*)info)
    );
}
SecureChannel::SecureChannel(const SecureChannel& other)
    : _socket(other._socket)
    , _session_key(other._session_key)
{}

int SecureChannel::Send(const std::vector<uint8_t>& message)
{
    AES::Encryption aesEncryption(_session_key.BytePtr(), _session_key.SizeInBytes());
    std::vector<uint8_t> ciphertext;

    // Generate IV
    AutoSeededRandomPool osrng;
    byte iv [AES::BLOCKSIZE];
    osrng.GenerateBlock(iv, AES::BLOCKSIZE);

    // Prepend IV
    ciphertext.insert(
        ciphertext.end(),
        reinterpret_cast<uint8_t*>(iv),
        iv + sizeof(iv)
    );

    // Append Body
    CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );
    StreamTransformationFilter stfEncryptor(cbcEncryption, new VectorSink(ciphertext));
    stfEncryptor.Put(reinterpret_cast<const unsigned char*>(message.data()), message.size());
    stfEncryptor.MessageEnd();

    // Write encrypted message to socket
    return _socket.SendBytes(ciphertext);
}

int SecureChannel::Receive(std::vector<uint8_t>& message)
{
    std::vector<uint8_t> output;

    // Receive encrypted message from socket
    std::vector<uint8_t> ciphertext; // TODO: Write and read message size
    std::size_t ciphertext_size = _socket.ReceiveBytes(ciphertext);
    if (ciphertext_size > 0)
    {
        // Decrypt Ciphertext
        AES::Decryption aesDecryption (_session_key.BytePtr(), _session_key.SizeInBytes());

        /// Copy IV
        byte iv [AES::BLOCKSIZE];
        memcpy(iv, ciphertext.data(), AES::BLOCKSIZE);

        /// Decrypt body
        CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
        StreamTransformationFilter stfDecryptor(cbcDecryption, new VectorSink(output));
        stfDecryptor.Put(
            reinterpret_cast<const unsigned char*>(ciphertext.data() + AES::BLOCKSIZE),
            ciphertext.size() - AES::BLOCKSIZE
        );
        stfDecryptor.MessageEnd();
    }

    message = output;
    return ciphertext_size;
}

TcpSocket SecureChannel::GetSocket()
{
    return _socket;
}