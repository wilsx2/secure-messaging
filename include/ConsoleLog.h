#ifndef CONSOLE_LOG_H
#define CONSOLE_LOG_H

#include "Logger.h"

class ConsoleLog : public LoggingTarget
{
    public:
    void Write(const std::string& message);
};

#endif