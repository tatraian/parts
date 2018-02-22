#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "logger.h"

namespace parts
{

class ConsoleLogger : public Logger {
public:
    ConsoleLogger(LOG_LEVELS min_level = LOG_LEVELS::INFO);
    ~ConsoleLogger() override = default;

    void log(LOG_LEVELS level, const char* filename, int line, const char* function, const std::string& msg) override;
protected:
    LOG_LEVELS m_minLevel;
};

}

#endif // CONSOLELOGGER_H
