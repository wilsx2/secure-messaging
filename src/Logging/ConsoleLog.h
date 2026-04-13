#ifndef CONSOLE_LOG_H
#define CONSOLE_LOG_H

#include "Logging/Logger.h"

class ConsoleLog : public LoggingTarget
{
    public:
    void Write(const std::string& message);
};

#endif