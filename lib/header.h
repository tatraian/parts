#ifndef HEADER_H
#define HEADER_H

#include "parts_definitions.h"
#include "contentreadbackend.h"

#include "inputbuffer.h"

namespace parts {


class Header
{
public:
    Header(const PartsCompressionParameters& parameters);
    Header(ContentReadBackend& reader);

    static const size_t HEADER_SIZE = 32;

    std::vector<uint8_t> getRaw() const;

    void setDecompressedTocSize(uint32_t size);
    uint32_t getDecompressedTocSize() const
    { return m_decompressedTocSize; }

    void setTocSize(uint32_t size);
    uint32_t getTocSize() const
    { return m_tocSize; }

    CompressionType getTocCompressionType() const
    { return m_tocCompressionType; }

    CompressionType getFileCompressionType() const
    { return m_fileCompressionType; }

    HashType getHashType() const
    { return m_hashType; }

    static bool checkMagic(const std::vector<uint8_t> & header, size_t offset);

protected:
    /** 32 bytes header */
    char* m_magic; // "parts!" - 6 bytes
    uint8_t m_dummy[14];
    uint8_t m_version;
    CompressionType m_tocCompressionType;
    CompressionType m_fileCompressionType;
    HashType m_hashType;
    uint32_t m_tocSize;
    uint32_t m_decompressedTocSize;
};

}

#endif // HEADER_H
