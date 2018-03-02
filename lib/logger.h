#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace parts {

enum class LOG_LEVELS{
    DEBUG = 0,
    TRACE = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4
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

}

#endif // LOGGER_H
