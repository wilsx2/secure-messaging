#include "ConsoleLog.h"
#include <iostream>

void ConsoleLog::Write(const std::string& message)
{
    std::cout << message;
}