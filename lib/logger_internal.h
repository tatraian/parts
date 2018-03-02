#ifndef LOGGER_INTERNAL_H
#define LOGGER_INTERNAL_H

#include "logger.h"

#include <fmt/format.h>

#define LOG_DEBUG(msg, ...)   if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))
#define LOG_TRACE(msg, ...)   if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::TRACE, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))
#define LOG_INFO(msg, ...)    if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::INFO, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))
#define LOG_WARNING(msg, ...) if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::WARNING, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))
#define LOG_ERROR(msg, ...)   if (Logger::__logger) Logger::__logger->log(LOG_LEVELS::ERROR, __FILE__, __LINE__, __FUNCTION__, fmt::format(msg, ##__VA_ARGS__))

#endif // LOGGER_INTERNAL_H
