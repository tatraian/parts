#include "consolelogger.h"

#include <iostream>

using namespace parts;

//==========================================================================================================================================
ConsoleLogger::ConsoleLogger(LOG_LEVELS min_level) :
    m_minLevel(min_level)
{
}

//==========================================================================================================================================
void ConsoleLogger::log(LOG_LEVELS level, const char* filename, int line, const char* function, const std::string& msg)
{
    if (level < m_minLevel)
        return;

    if (level >= LOG_LEVELS::WARNING) {
        std::cerr << msg << std::endl;
    } else {
        std::cout << msg << std::endl;
    }
}
