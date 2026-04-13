#include "Server/Server.h"
#include "Logging/Logger.h"
#include "Logging/ConsoleLog.h"

int main()
{
    auto& logger = Logger::GetInstance();
    logger.SetTarget(std::make_unique<ConsoleLog>());
    logger.SetLevel(Logger::Level::Info);
    
    Server server;
    server.Run();

    return 0;
}