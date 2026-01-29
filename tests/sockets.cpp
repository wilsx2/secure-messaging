#include "TcpSocket.h"
#include <thread>
#include <iostream>
#include <unistd.h>

constexpr int PORT = 5555;

void server()
{
    TcpSocket serverSocket(PORT, INADDR_ANY);

    serverSocket.Bind();
    serverSocket.Listen(1);

    TcpSocket client = serverSocket.Accept();

    std::string msg = client.Read(1024);
    std::cout << "[Server] Received: " << msg << std::endl;

    client.Write("Hello from server");
}

void client()
{
    // Give server time to start
    sleep(1);

    TcpSocket clientSocket(PORT, INADDR_LOOPBACK);
    clientSocket.Connect();

    clientSocket.Write("Hello from client");

    std::string response = clientSocket.Read(1024);
    std::cout << "[Client] Received: " << response << std::endl;
}

int main()
{
    std::thread serverThread(server);
    std::thread clientThread(client);

    serverThread.join();
    clientThread.join();

    return 0;
}
