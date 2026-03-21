#include "server/Server.h"
#include "client/Client.h"
#include "client/MessageBuilder.h"
#include "logging/Logger.h"
#include "logging/ConsoleLog.h"
#include <atomic>
#include <thread>

std::atomic<bool> server_running = false;

int main()
{
    auto& logger = Logger::GetInstance();
    logger.SetTarget(std::make_unique<ConsoleLog>());
    logger.SetLevel(Logger::Level::Info);

    std::thread server_thread ([&](){
        Server server;
        server_running = true;
        while(server.HandleEvents() && server_running);
    });

    {
        logger.Debug("Opening client session");

        Client client;
        client.Connect();
        client.SendRequest(MessageBuilder::Build("register alice_smith fizzBuzz123$"));
        client.AwaitResponse();
        client.SendRequest(MessageBuilder::Build("login alice_smith fizzBuzz123$"));
        client.AwaitResponse();
        client.SendRequest(MessageBuilder::Build("send alice_smith love"));
        client.AwaitResponse();
        client.AwaitResponse();

        logger.Debug("Closing client session");
    }

    server_running = false;
    server_thread.join();
    return 0;
}