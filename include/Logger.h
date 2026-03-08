#ifndef LOGGER_H
#define LOGGER_H
#include <memory>
#include <string>

class LoggingTarget
{
    public:
    virtual void Write(const std::string& message) = 0;
};

class Logger
{
    private:
    enum class Level { Fatal, Error, Warn, Info, Debug, Trace };
    Level _min_level;
    std::unique_ptr<LoggingTarget> _target;
    void Write(Level log_level, const std::string& message);

    public:
    Logger(std::unique_ptr<LoggingTarget>&& target, Level min_level);
    void SetLevel(Level min_level);
    Level GetLevel();
    void Fatal(const std::string& message);
    void Error(const std::string& message);
    void Info(const std::string& message);
    void Warn(const std::string& message);
    void Info(const std::string& message);
    void Debug(const std::string& message);
    void Trace(const std::string& message);
};

#endif