#include "SecureChannel.h"

#include <iostream>
#include <stdexcept>
#include <cryptopp/eccrypto.h>
#include <cryptopp/integer.h>
#include <cryptopp/oids.h>
#include <cryptopp/osrng.h>
#include <cryptopp/asn.h>
#include <cryptopp/secblock.h>

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

    // Output
    Integer ss;
    ss.Decode(_sessionKey.BytePtr(), _sessionKey.SizeInBytes());
    std::cout << "Shared secret: " << std::hex << ss << std::endl;
}