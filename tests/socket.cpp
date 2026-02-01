#include "Global.h"
#include "TcpSocket.h"

int main()
{
    TcpSocket server (PORT, INADDR_ANY);
    server.Bind();
    server.Listen(1);

    TcpSocket client_to_server (PORT, INADDR_LOOPBACK);
    client_to_server.Connect();

    TcpSocket server_to_client (server.Accept());

    std::string sent_message = "Peace and love on planet Earth.";
    std::vector<uint8_t> data (sent_message.begin(), sent_message.end());
    server_to_client.SendBytes(data);
    client_to_server.ReceiveBytes(data);
    std::string received_message (data.begin(), data.end());

    server_to_client.Close();
    client_to_server.Close();
    server.Close();

    std::cout << "Sent: " << sent_message << std::endl;
    std::cout << "Recv: " << received_message << std::endl;
}