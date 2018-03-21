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
                     CompressionType compression_type,
                     uint64_t uncompressed_size);

    RegularFileEntry(InputBuffer& buffer,
                     const std::vector<std::string>& owners,
                     const std::vector<std::string>& groups,
                     HashType hash_type,
                     CompressionType compression_type);

    ~RegularFileEntry() override = default;

    void append(std::vector<uint8_t>& buffer) const override;

    void compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend) override;

    void extractEntry(const boost::filesystem::path& dest_root, Decompressor& decompressor, ContentReadBackend& backend) override;

    void updateEntry(const BaseEntry* old_entry,
                     const boost::filesystem::path& old_root,
                     const boost::filesystem::path& dest_root,
                     Decompressor& decompressor,
                     ContentReadBackend& backend,
                     bool checkExisting) override;

    std::string listEntry(size_t user_width, size_t size_width, std::tm* t) const override;

    bool extractToMc(const boost::filesystem::path& dest_file, Decompressor& decompressor, ContentReadBackend& backend) override;

    std::string toString() const override;

    const Hash& uncompressedHash() const
    { return m_uncompressedHash; }

    const uint64_t& uncompressedSize() const
    { return m_uncompressedSize; }

    /**
     * This is purely for background compatibility, in case it turns out that
     * the given file was stored as not compressed (.VER)
     */
    CompressionType compressionType() const
    { return m_compressionType; }

    const uint64_t& compressedSize() const
    { return m_compressedSize; }

    const uint64_t& offset() const
    { return m_offset; }

    void shiftOffset(uint64_t& shift)
    { m_offset += shift; }



protected:
    Hash m_uncompressedHash;
    uint64_t m_uncompressedSize;
    CompressionType m_compressionType;
    uint64_t m_compressedSize;
    uint64_t m_offset;
};

}
#endif // REGULARFILEENTRY_H
