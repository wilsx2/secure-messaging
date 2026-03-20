#include "server/Server.h"
#include "client/ClientSession.h"
#include "logging/Logger.h"
#include "logging/ConsoleLog.h"
#include <atomic>
#include <thread>

std::atomic<bool> server_running = false;

int main()
{
    auto& logger = Logger::GetInstance();
    logger.SetTarget(std::make_unique<ConsoleLog>());

    std::thread server_thread ([&](){
        Server server;
        server_running = true;
        while(server.HandleEvents() && server_running);
    });

    {
        ClientSession client;
        client.Connect();
        client.SendCommand("register alice_smith fizzBuzz123$");
        client.AwaitResponse();
        client.SendCommand("login alice_smith fizzBuzz123$");
        client.AwaitResponse();
        client.SendCommand("send alice_smith love");
        client.AwaitResponse();
    }

    server_running = false;
    server_thread.join();
    return 0;
}