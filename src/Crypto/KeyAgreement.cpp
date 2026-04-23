#include "Crypto/KeyAgreement.h"
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

KeyAgreement::KeyAgreement()
    : _dh(ASN1::secp256r1())
    , _private_key(_dh.PrivateKeyLength())
    , _public_key(_dh.PublicKeyLength())
    , _other_key(_dh.PublicKeyLength())
{
    _dh.GenerateKeyPair(_rng, _private_key, _public_key);
}

void* KeyAgreement::GetPublicKey(std::size_t& length)
{
    length = _public_key.size();
    return _public_key.data();
}
void* KeyAgreement::GetOtherKey(std::size_t& length)
{
    length = _other_key.size();
    return _other_key.data();
}
void* KeyAgreement::GetSalt(std::size_t& length)
{
    length = _salt.size();
    return _salt.data();
}
void KeyAgreement::GenerateSalt(std::size_t length)
{
    _salt.New(length);
    _rng.GenerateBlock(_salt.data(), _salt.size());
}

std::optional<SecureKey> KeyAgreement::Agree()
{
    // Key Agreement
    SecByteBlock shared(_dh.AgreedValueLength());
    if(!_dh.Agree(shared, _private_key, _other_key))
        return std::nullopt;

    // Key Derivation
    HKDF<SHA256> hkdf;
    byte info[] = "secure messaging";

    SecByteBlock derived(shared.size());
    hkdf.DeriveKey(
        derived.BytePtr(), derived.SizeInBytes(), 
        shared.BytePtr(), shared.SizeInBytes(),
        _salt.BytePtr(), _salt.SizeInBytes(), 
        info, strlen((const char*)info)
    );

    return SecureKey(std::move(derived));
}