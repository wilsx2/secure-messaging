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

void KeyAgreement::GetPublicKey(uint8_t*& data, std::size_t& size)
{
    data = _public_key.data();
    size = _public_key.size();
}
void KeyAgreement::GetOtherKey(uint8_t*& data, std::size_t& size)
{
    data = _other_key.data();
    size = _other_key.size();
}
void KeyAgreement::GetSalt(uint8_t*& data, std::size_t& size)
{
    data = _salt.data();
    size = _salt.size();
}
void KeyAgreement::GenerateSalt(std::size_t size)
{
    _salt.New(size);
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