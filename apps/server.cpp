#include "server/Server.h"
#include "logging/Logger.h"
#include "logging/ConsoleLog.h"

int main()
{
    auto& logger = Logger::GetInstance();
    logger.SetTarget(std::make_unique<ConsoleLog>());
    logger.SetLevel(Logger::Level::Info);
    
    Server server;
    server.Run();

    return 0;
}