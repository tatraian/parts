#ifndef PARTS_DEFINITIONS_H
#define PARTS_DEFINITIONS_H

#include <stdexcept>

namespace parts
{

enum class CompressionType {
    None = 0,
    LZMA = 1,
};

enum class HashType {
    MD5 = 0,
    SHA256 = 1,
};

class PartsException : public std::runtime_error {
public:
    PartsException(const std::string& msg) : std::runtime_error(msg) {}
};

}

#endif // PARTS_DEFINITIONS_H
