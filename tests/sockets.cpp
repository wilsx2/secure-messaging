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

    char buff[1024];
    client.Read(buff, sizeof(buff));
    std::cout << "[Server] Received: " << std::string(buff) << std::endl;

    char msg[] = "Hello from server";
    client.Write(msg, sizeof(msg));

    client.Close();
    serverSocket.Close();
}

void client()
{
    // Give server time to start
    sleep(1);

    TcpSocket clientSocket(PORT, INADDR_LOOPBACK);
    clientSocket.Connect();

    char msg[] = "Hello from client";
    clientSocket.Write(msg, sizeof(msg));

    char buff[1024];
    clientSocket.Read(buff, sizeof(buff));
    std::cout << "[Client] Received: " << std::string(buff) << std::endl;

    clientSocket.Close();
}

int main()
{
    std::thread serverThread(server);
    std::thread clientThread(client);

    serverThread.join();
    clientThread.join();

    return 0;
}
