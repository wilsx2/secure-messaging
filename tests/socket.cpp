#include "Shared/Global.h"
#include "Network/TcpListener.h"
#include "Network/TcpClient.h"
#include <cassert>

int main()
{
    TcpListener listener;
    assert(listener.Listen(IpAddress::LocalHost(), PORT));

    TcpClient client_to_server;
    assert(client_to_server.Connect(IpAddress::LocalHost(), PORT));

    TcpClient server_to_client (std::move(listener.Accept().value()));

    std::string sent_msg = "Peace and love on planet Earth.";
    std::size_t sent_len = sent_msg.length();
    std::string recv_msg;
    std::size_t recv_len;
    assert(server_to_client.Send(&sent_len, sizeof(sent_len)) == sizeof(sent_len));
    assert(server_to_client.Send(sent_msg.data(), sent_len) == sent_len);
    assert(client_to_server.Receive(&recv_len, sizeof(recv_len)) == sizeof(recv_len));
    recv_msg.resize(recv_len);
    assert(client_to_server.Receive(recv_msg.data(), recv_len) == recv_len);
    assert(sent_msg == recv_msg);
    return 0;
}