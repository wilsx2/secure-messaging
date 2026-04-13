#pragma once

#include "Logging/Logger.h"

class ConsoleLog : public LoggingTarget
{
    public:
    void Write(const std::string& message);
};