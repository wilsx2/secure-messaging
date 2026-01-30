#include "TcpSocket.h"
#include <thread>
#include <iostream>
#include <unistd.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/integer.h>
#include <cryptopp/oids.h>
#include <cryptopp/osrng.h>
#include <cryptopp/asn.h>
#include <cryptopp/secblock.h>

using namespace std;
using namespace CryptoPP;

constexpr int PORT = 5555;

void server()
{
    OID CURVE = ASN1::secp256r1();
    AutoSeededRandomPool rng;
    ECDH < ECP >::Domain dh( CURVE );
    SecByteBlock priv(dh.PrivateKeyLength()), pub(dh.PublicKeyLength()), pubClient(dh.PublicKeyLength());
    dh.GenerateKeyPair(rng, priv, pub);
    SecByteBlock shared(dh.AgreedValueLength());

    TcpSocket serverSocket(PORT, INADDR_ANY);

    serverSocket.Bind();
    serverSocket.Listen(1);

    TcpSocket client = serverSocket.Accept();

    client.Read(pubClient.BytePtr(), pubClient.SizeInBytes()); // Receive public key
    client.Write(pub.BytePtr(), pub.SizeInBytes()); // Send public key

    if(!dh.Agree(shared, priv, pubClient))
    {
        cout << "[Server] Failed to reach shared secret" << endl;
        return;
    }
    Integer ss;
    ss.Decode(shared.BytePtr(), shared.SizeInBytes());
    cout << "[Server] Shared secret: " << hex << ss << endl;

    client.Close();
    serverSocket.Close();
}

void client()
{
    OID CURVE = ASN1::secp256r1();
    AutoSeededRandomPool rng;
    ECDH < ECP >::Domain dh( CURVE );
    SecByteBlock priv(dh.PrivateKeyLength()), pub(dh.PublicKeyLength()), pubServer(dh.PublicKeyLength());
    dh.GenerateKeyPair(rng, priv, pub);
    SecByteBlock shared(dh.AgreedValueLength());

    // Give server time to start
    sleep(1);

    TcpSocket clientSocket(PORT, INADDR_LOOPBACK);
    clientSocket.Connect();

    clientSocket.Write(pub.BytePtr(), pub.SizeInBytes()); // Send public key
    clientSocket.Read(pubServer.BytePtr(), pubServer.SizeInBytes()); // Receive public key
    
    if(!dh.Agree(shared, priv, pubServer))
    {
        cout << "[Client] Failed to reach shared secret" << endl;
        return;
    }

    Integer ss;
    ss.Decode(shared.BytePtr(), shared.SizeInBytes());
    cout << "[Client] Shared secret: " << hex << ss << endl;

    clientSocket.Close();
}

int main()
{
    thread serverThread(server);
    thread clientThread(client);

    serverThread.join();
    clientThread.join();

    return 0;
}
