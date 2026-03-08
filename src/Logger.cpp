#include "Logger.h"

Logger::Logger(std::unique_ptr<LoggingTarget>&& target, Level min_level)
    : _target(target.release())
    , _min_level(min_level)
{}

void Logger::SetLevel(Level min_level)
{
    _min_level = min_level;
}

Logger::Level Logger::GetLevel()
{
    return _min_level;
}

void Logger::Write(Level log_level, const std::string& message)
{
    if (log_level < _min_level)
        return;
    switch (log_level)
    {
        case Level::Fatal:  _target->Write("[FATAL] "); break;
        case Level::Error:  _target->Write("[ERROR] "); break;
        case Level::Warn:   _target->Write("[WARN]  "); break;
        case Level::Info:   _target->Write("[INFO]  "); break;
        case Level::Debug:  _target->Write("[DEBUG] "); break;
        case Level::Trace:  _target->Write("[TRACE] "); break;
    }

    _target->Write(message);
    _target->Write("\n");
}

void Logger::Fatal(const std::string& message)
{
    Write(Level::Fatal, message);
}
void Logger::Error(const std::string& message)
{
    Write(Level::Error, message);
}
void Logger::Info(const std::string& message)
{
    Write(Level::Info, message);
}
void Logger::Warn(const std::string& message)
{
    Write(Level::Warn, message);
}
void Logger::Info(const std::string& message)
{
    Write(Level::Info, message);
}
void Logger::Debug(const std::string& message)
{
    Write(Level::Debug, message);
}
void Logger::Trace(const std::string& message)
{
    Write(Level::Trace, message);
}
