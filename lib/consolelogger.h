#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "logger_internal.h"

namespace parts
{

class ConsoleLogger : public Logger {
public:
    ConsoleLogger(LOG_LEVELS min_level = LOG_LEVELS::INFO, bool developper = false);
    ~ConsoleLogger() override = default;

    void log(LOG_LEVELS level, const char* filename, int line, const char* function, const std::string& msg) override;
protected:
    LOG_LEVELS m_minLevel;
    bool m_developper;
};

}

#endif // CONSOLELOGGER_H
