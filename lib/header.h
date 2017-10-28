#ifndef HEADER_H
#define HEADER_H

#include "parts_definitions.h"

#include <deque>

namespace parts {


class Header
{
public:
    Header(const PartsCompressionParameters& parameters);

    std::vector<uint8_t> getRaw() const;

    void setTocSize(uint32_t size);

protected:
    char* m_magic; // "parts!" - 6 bytes
    uint8_t m_version;
    CompressionType m_tocCompressionType;
    CompressionType m_fileCompressionType;
    HashType m_hashType;
    uint8_t m_dummy[2];
    uint32_t m_tocSize;
};

}

#endif // HEADER_H
