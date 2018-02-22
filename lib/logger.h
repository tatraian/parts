#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fmt/format.h>

namespace parts {

enum class LOG_LEVELS{
    DEBUG,
    TRACE,
    INFO,
    WARNING,
    ERROR
};

constexpr const char* to_string(LOG_LEVELS level) {
    switch (level) {
        case LOG_LEVELS::DEBUG:
            return "DEBUG";
        case LOG_LEVELS::TRACE:
            return "TRACE";
        case LOG_LEVELS::INFO:
            return "INFO";
        case LOG_LEVELS::WARNING:
            return "WARNING";
        case LOG_LEVELS::ERROR:
            return "ERROR";
    }
    return "";
}

class Logger
{
public:
    Logger() = default;
    virtual ~Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    virtual void log(LOG_LEVELS level, const char* filename, int line, const char* function, const std::string& msg) = 0;

    static Logger* __logger;
};


inline void set_logger(Logger* logger)
{ Logger::__logger = logger; }

#define LOG_DEBUG(msg, ...)   if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))
#define LOG_TRACE(msg, ...)   if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::TRACE, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))
#define LOG_INFO(msg, ...)    if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::INFO, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))
#define LOG_WARNING(msg, ...) if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::WARNING, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))
#define LOG_ERROR(msg, ...)   if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::ERROR, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))

}

#endif // LOGGER_H
