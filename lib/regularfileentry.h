#ifndef REGULARFILEENTRY_H
#define REGULARFILEENTRY_H

#include "baseentry.h"
#include "hash.h"

namespace parts
{

class RegularFileEntry : public BaseEntry
{
public:
    RegularFileEntry(const boost::filesystem::path& file,
                     uint16_t permissions,
                     const std::string& owner,
                     uint16_t owner_id,
                     const std::string& group,
                     uint16_t group_id,
                     Hash uncompressed_hash,
                     uint64_t uncompressed_size);

    ~RegularFileEntry() override = default;

    void append(std::vector<uint8_t>& buffer) const override;

    void compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend) override;

    const Hash& uncompressedHash() const
    { return m_uncompressedHash; }

    const uint64_t& uncompressedSize() const
    { return m_uncompressedSize; }

    const uint64_t& compressedSize() const
    { return m_compressedSize; }

    const uint64_t& offset() const
    { return m_offset; }

protected:
    Hash m_uncompressedHash;
    uint64_t m_uncompressedSize;
    uint64_t m_compressedSize;
    uint64_t m_offset;
};

}
#endif // REGULARFILEENTRY_H
