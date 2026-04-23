#include "Crypto/Hash.h"
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

void Hash(void* dest, const void* src, std::size_t length)
{
    SHA256 hash;

    ArraySource source (
        reinterpret_cast<const byte*>(src), length, true, 
        new HashFilter(hash,
            new HexEncoder(
                new ArraySink(reinterpret_cast<byte*>(dest), length)
            )
        )
    );
}