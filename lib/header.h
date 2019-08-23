#ifndef HEADER_H
#define HEADER_H

#include "parts_definitions.h"
#include "contentreadbackend.h"

#include "inputbuffer.h"

namespace parts {


class Header
{
public:
    Header(const PartsCompressionParameters& parameters) noexcept;
    Header(ContentReadBackend& reader) noexcept;

    Header(const Header&) = default;
    Header& operator=(const Header&) = default;

    Header(Header&&) = default;
    Header& operator=(Header&&) = default;

    std::vector<uint8_t> getRaw() const;

    void setTocSize(uint32_t size);
    uint32_t getTocSize() const
    { return m_tocSize; }

    CompressionType getTocCompressionType() const
    { return m_tocCompressionType; }

    HashType getHashType() const
    { return m_hashType; }

    operator bool() const
    { return m_valid; }

protected:
    bool m_valid;
    char* m_magic; // "parts!" - 6 bytes
    uint8_t m_version;
    CompressionType m_tocCompressionType;
    HashType m_hashType;
    uint8_t m_dummy[3];
    uint32_t m_tocSize;
};

}

#endif // HEADER_H
