#include "network/SecureChannel.h"
#include "logging/Logger.h"

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

SecureChannel::SecureChannel(TcpSocket&& socket)
    : _socket(std::move(socket))
    , _session_key(AES::DEFAULT_KEYLENGTH)
{}
SecureChannel::SecureChannel(SecureChannel&& other)
    : _socket(std::move(other._socket))
    , _session_key(other._session_key)
{}

bool SecureChannel::EstablishKey(HostType host_type)
{
    // Key Agreement
    OID CURVE = ASN1::secp256r1();
    AutoSeededRandomPool rng;
    ECDH < ECP >::Domain dh( CURVE );
    SecByteBlock priv(dh.PrivateKeyLength()), pub(dh.PublicKeyLength()), pubOther(dh.PublicKeyLength());
    dh.GenerateKeyPair(rng, priv, pub);
    SecByteBlock shared(dh.AgreedValueLength());
    SecByteBlock salt(256);

    if (host_type == HostType::Client)
    {   
        if (_socket.Connect() == -1)
        {
            Logger::GetInstance().Error("[SecureChannel] Failed to connect socket");
            return false;
        }
        _socket.Send(pub.BytePtr(), pub.SizeInBytes(), 0);
        _socket.Receive(pubOther.BytePtr(), pubOther.SizeInBytes(), 0);
        rng.GenerateBlock(salt.BytePtr(), salt.SizeInBytes());
        _socket.Send(salt.BytePtr(), salt.SizeInBytes(), 0);
        
    }
    else // host_type == HostType::Server
    {
        _socket.Receive(pubOther.BytePtr(), pubOther.SizeInBytes(), 0);
        _socket.Send(pub.BytePtr(), pub.SizeInBytes(), 0);
        _socket.Receive(salt.BytePtr(), salt.SizeInBytes(), 0);
    }
    
    if(!dh.Agree(shared, priv, pubOther))
    {
        Logger::GetInstance().Error("[SecureChannel] Failed to agree on shared key");
        return false;
    } else {
        Logger::GetInstance().Info("[SecureChannel] Agreed on shared key");

    }

    // Key Derivation
    HKDF<SHA256> hkdf;
    byte info[] = "secure messaging";

    hkdf.DeriveKey(
        _session_key.BytePtr(), _session_key.SizeInBytes(), 
        shared.BytePtr(), shared.SizeInBytes(),
        salt.BytePtr(), salt.SizeInBytes(), 
        info, strlen((const char*)info)
    );

    return true;
}

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
    std::vector<uint8_t> ciphertext;
    int ciphertext_size = _socket.ReceiveBytes(ciphertext);
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

        message = output;
    }

    return ciphertext_size;
}

const TcpSocket& SecureChannel::GetSocket()
{
    return _socket;
}