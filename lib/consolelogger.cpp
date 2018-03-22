#include "consolelogger.h"

#include <iostream>
#include <fmt/format.h>

using namespace parts;

//==========================================================================================================================================
ConsoleLogger::ConsoleLogger(LOG_LEVELS min_level, bool developper) :
    m_minLevel(min_level),
    m_developper(developper)
{
}

//==========================================================================================================================================
void ConsoleLogger::log(LOG_LEVELS level, const char* filename, int line, const char* function, const std::string& msg)
{
    if (level < m_minLevel)
        return;

    if (!m_developper) {
        if (level >= LOG_LEVELS::WARNING) {
            std::cerr << msg << std::endl;
        } else {
            std::cout << msg << std::endl;
        }
    } else {
        std::cerr << fmt::format("{:>20}:{} {}", filename, line, msg) << std::endl;
    }
}
