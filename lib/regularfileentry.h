#ifndef REGULARFILEENTRY_H
#define REGULARFILEENTRY_H

#include "baseentry.h"
#include "hash.h"

namespace parts
{

class RegularFileEntry : public BaseEntry
{
public:
    /** 
     * We store root-relative filenames (relfile), however for the hash generation to work
     * we need a kind of absolute path (file)
     */
    RegularFileEntry(const boost::filesystem::path& relfile,
                     const boost::filesystem::path& file,
                     uint16_t permissions,
                     const std::string& owner,
                     uint16_t owner_id,
                     const std::string& group,
                     uint16_t group_id,
                     HashType hash_type,
                     CompressionType compression_hint,
                     const PartsCompressionParameters & compression_parameters,
                     uint64_t uncompressed_size);

    /** 
     * This variant is required so that we can setup the hash from outside 
     * E.g. we have a NOT Parts archive, from which we read hash from
     **/
    RegularFileEntry(const boost::filesystem::path& relfile,
                     const boost::filesystem::path& file,
                     uint16_t permissions,
                     const std::string& owner,
                     uint16_t owner_id,
                     const std::string& group,
                     uint16_t group_id,
                     const Hash & uncompressed_hash,
                     CompressionType compression_hint,
                     const PartsCompressionParameters & compression_parameters,
                     uint64_t uncompressed_size);

    RegularFileEntry(InputBuffer& buffer,
                     const std::vector<std::string>& owners,
                     const std::vector<std::string>& groups,
                     HashType hash_type,
                     CompressionType compression_hint,
                     const PartsCompressionParameters & compression_parameters);

    ~RegularFileEntry() override = default;

    void append(std::vector<uint8_t>& buffer) const override;

    void compressEntry(const boost::filesystem::path& root, ContentWriteBackend& backend) override;

    void extractEntry(const boost::filesystem::path& dest_root, ContentReadBackend& backend) override;

    void updateEntry(const BaseEntry* old_entry,
                     const boost::filesystem::path& old_root,
                     const boost::filesystem::path& dest_root,
                     ContentReadBackend& backend,
                     bool checkExisting) override;

    std::string listEntry(size_t user_width, size_t size_width, std::tm* t) const override;

    bool extractToMc(const boost::filesystem::path& dest_file, ContentReadBackend& backend) override;

    std::string toString() const override;

    const Hash& uncompressedHash() const
    { return m_uncompressedHash; }

    const uint64_t& uncompressedSize() const
    { return m_uncompressedSize; }

    /**
     * This is purely for background compatibility, in case it turns out that
     * the given file was stored as not compressed (.VER)
     */
    CompressionType compressionHint() const
    { return m_compressionHint; }

    const uint64_t& compressedSize() const
    { return m_compressedSize; }

    const uint64_t& offset() const
    { return m_offset; }

    void shiftOffset(uint64_t& shift)
    { m_offset += shift; }



protected:
    Hash m_uncompressedHash;
    uint64_t m_uncompressedSize;
    CompressionType m_compressionHint;
    PartsCompressionParameters m_compressionParameters;
    uint64_t m_compressedSize;
    uint64_t m_offset;
};

}
#endif // REGULARFILEENTRY_H
