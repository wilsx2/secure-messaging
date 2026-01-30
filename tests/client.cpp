#include "Global.h"
#include "TcpSocket.h"
#include "SecureChannel.h"

int main()
{
    TcpSocket clientSocket(PORT, INADDR_LOOPBACK);
    clientSocket.Connect();

    SecureChannel channel (clientSocket, HostType::Client);

    while (true)
    {
        std::string output;
        std::cin >> output;
        channel.Send(output);
        std::string message = channel.Receive();
        std::cout << "[Client] Received: " << message << std::endl;
    }

    clientSocket.Close();
}