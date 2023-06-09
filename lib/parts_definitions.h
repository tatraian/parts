#ifndef PARTS_DEFINITIONS_H
#define PARTS_DEFINITIONS_H

#include <cstdint>
#include <stdexcept>
#include <string>

namespace parts
{

enum class CompressionType : uint8_t {
    None = 0,
    LZMA = 1,
    ZLIB = 2,
};

enum class HashType {
    MD5 = 0,
    SHA256 = 1,
};

#if __GNUG__
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

inline constexpr const char* to_string(CompressionType type) {
    switch (type) {
    case CompressionType::None:
        return "Plain";
    case CompressionType::LZMA:
        return "LZMA";
    case CompressionType::ZLIB:
        return "ZLIB";
    }
}

class PartsException : public std::runtime_error {
public:
    PartsException(const std::string& msg) : std::runtime_error(msg) {}
};

inline constexpr size_t hash_size(HashType type) {
    switch (type) {
    case HashType::MD5:
        return 16;
    case HashType::SHA256:
        return 32;
    }
}

#if __GNUG__
#pragma GCC diagnostic error "-Wreturn-type"
#endif

struct LzmaCompressorParameters
{
    LzmaCompressorParameters(uint32_t compression_level = 6,
                             bool x86Filter = true) :
        m_compressionLevel(compression_level),
        m_x86FilterActive(x86Filter)
    {}

    uint32_t m_compressionLevel;
    bool m_x86FilterActive;
    // currently threads are not supported since stream_encoder_mt is not in hunter base package (cannot be compiled...)
    //uint32_t m_threads;
};

struct PartsCompressionParameters
{
    PartsCompressionParameters(HashType hash_type = HashType::SHA256,
                               CompressionType toc_compression = CompressionType::LZMA,
                               CompressionType file_compression = CompressionType::LZMA,
                               bool save_owners = false) :
        m_hashType(hash_type),
        m_tocCompression(toc_compression),
        m_fileCompression(file_compression),
        m_saveOwners(save_owners)
    {}

    HashType m_hashType;
    CompressionType m_tocCompression;
    CompressionType m_fileCompression;
    bool m_saveOwners;

    LzmaCompressorParameters m_lzmaParameters;
};

}

#endif // PARTS_DEFINITIONS_H
