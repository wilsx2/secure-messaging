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
    public: enum class Level { Fatal, Error, Warn, Info, Debug, Trace };

    private:
    static Logger _instance;
    std::unique_ptr<LoggingTarget> _target;
    Level _min_level;
    Logger(std::unique_ptr<LoggingTarget>&& target, Level min_level);
    void Write(Level log_level, const std::string& message);

    public:
    static Logger& GetInstance();
    void SetTarget(std::unique_ptr<LoggingTarget>&& target);
    void SetLevel(Level min_level);
    Level GetLevel();
    void Fatal(const std::string& message);
    void Error(const std::string& message);
    void Warn(const std::string& message);
    void Info(const std::string& message);
    void Debug(const std::string& message);
    void Trace(const std::string& message);
};

#endif