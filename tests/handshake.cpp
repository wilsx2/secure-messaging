#include "Server.h"
#include "ClientSession.h"
#include <atomic>
#include <thread>

std::atomic<bool> server_running = false;

int main()
{
    std::thread server_thread ([&](){
        Server server;
        server_running = true;
        while(server.HandleEvents() && server_running);
    });

    {
        ClientSession client;
        client.Connect();
        client.SendCommand("login alice");
        client.AwaitResponse();
        client.SendCommand("send alice love");
        client.AwaitResponse();
    }

    std::cout << "end \n";

    server_running = false;
    server_thread.join();
    return 0;
}