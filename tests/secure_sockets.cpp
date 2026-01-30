#include "SecureChannel.h"
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
    TcpSocket serverSocket(PORT, INADDR_ANY);

    serverSocket.Bind();
    serverSocket.Listen(1);

    TcpSocket client = serverSocket.Accept();

    SecureChannel channel (client, HostType::Server);

    client.Close();
    serverSocket.Close();
}

void client()
{
    // Give server time to start
    sleep(1);

    TcpSocket clientSocket(PORT, INADDR_LOOPBACK);
    clientSocket.Connect();

    SecureChannel channel (clientSocket, HostType::Client);


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
