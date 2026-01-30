#include <iostream>
#include <stdexcept>
#include <cryptopp/eccrypto.h>
#include <cryptopp/integer.h>
#include <cryptopp/oids.h>
#include <cryptopp/osrng.h>
#include <cryptopp/asn.h>
#include <cryptopp/secblock.h>

using namespace CryptoPP;
using namespace std;

int main(int argc, char* argv[]) {
    OID CURVE = ASN1::secp256r1();
    AutoSeededRandomPool rng;

    ECDH < ECP >::Domain dhA( CURVE ), dhB( CURVE );
    SecByteBlock privA(dhA.PrivateKeyLength()), pubA(dhA.PublicKeyLength());
    SecByteBlock privB(dhB.PrivateKeyLength()), pubB(dhB.PublicKeyLength());

    dhA.GenerateKeyPair(rng, privA, pubA);
    dhB.GenerateKeyPair(rng, privB, pubB);

    if(dhA.AgreedValueLength() != dhB.AgreedValueLength())
        throw runtime_error("Shared shared size mismatch");

    SecByteBlock sharedA(dhA.AgreedValueLength()), sharedB(dhB.AgreedValueLength());

    if(!dhA.Agree(sharedA, privA, pubB))
        throw runtime_error("Failed to reach shared secret (A)");

    if(!dhB.Agree(sharedB, privB, pubA))
        throw runtime_error("Failed to reach shared secret (B)");

    Integer ssa, ssb;

    ssa.Decode(sharedA.BytePtr(), sharedA.SizeInBytes());
    cout << "(A): " << hex << ssa << endl;

    ssb.Decode(sharedB.BytePtr(), sharedB.SizeInBytes());
    cout << "(B): " << hex << ssb << endl;

    if(ssa != ssb)
        throw runtime_error("Failed to reach shared secret (C)");

    cout << "Agreed to shared secret" << endl;

    return 0;
}
