#include "SecureChannel.h"
#include "TcpSocket.h"
#include "Server.h"
#include <thread>
#include <iostream>

using namespace std;
using namespace CryptoPP;

constexpr int PORT = 5555;

void server()
{
    Server server;
    server.Run();
}

void client()
{
    // Give server time to start
    sleep(1);

    TcpSocket clientSocket(PORT, INADDR_LOOPBACK);
    clientSocket.Connect();

    SecureChannel channel (clientSocket, HostType::Client);

    channel.Send("Hello from client");
    std::string message = channel.Receive();
    std::cout << "[Client] Received: " << message << std::endl;

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