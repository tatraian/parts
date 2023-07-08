#ifndef REGULARFILEENTRY_H
#define REGULARFILEENTRY_H

#include "baseentry.h"
#include "hash.h"
#include "parts_definitions.h"
#include "compressor.h"

namespace parts
{

class RegularFileEntry : public BaseEntry
{
public:
    RegularFileEntry(const std::filesystem::path& root,
                     const std::filesystem::path& file,
                     std::vector<std::string>& owners,
                     std::vector<std::string>& groups,
                     bool save_owner,
                     PartsCompressionParameters compression_parameters) noexcept;

    RegularFileEntry(InputBuffer& buffer,
                     const std::vector<std::string>& owners,
                     const std::vector<std::string>& groups,
                     HashType hash_type) noexcept;

    ~RegularFileEntry() noexcept override = default;

    RegularFileEntry(const RegularFileEntry&) = default;
    RegularFileEntry& operator=(const RegularFileEntry&) = default;

    RegularFileEntry(RegularFileEntry&&) = default;
    RegularFileEntry& operator=(RegularFileEntry&&) = default;

    void append(std::vector<uint8_t>& buffer) const override;

    void compressEntry(ContentWriteBackend& backend) override;

    void extractEntry(const std::filesystem::path& dest_root, ContentReadBackend& backend, bool cont) override;

    void updateEntry(const BaseEntry* old_entry,
                     const std::filesystem::path& old_root,
                     const std::filesystem::path& dest_root,
                     ContentReadBackend& backend,
                     bool cont) override;

    std::string listEntry(size_t user_width, size_t size_width, std::tm* t) const override;

    bool extractToMc(const std::filesystem::path& dest_file, ContentReadBackend& backend) override;

    std::string toString() const override;

    const Hash& uncompressedHash() const
    { return m_uncompressedHash; }

    const uint64_t& uncompressedSize() const
    { return m_uncompressedSize; }

    const uint64_t& compressedSize() const
    { return m_compressedSize; }

    const uint64_t& offset() const
    { return m_offset; }

    void shiftOffset(uint64_t& shift)
    { m_offset += shift; }

    /**
     * @brief compressionType gets back the compression type. Before compressing only the hint is send back. In case of small files this can
     * be changed to plain.
     */
    CompressionType compressionType() const
    { return m_compressionType; }

protected:
    // help fake unit test
    RegularFileEntry(const std::filesystem::path& root,
                     const std::filesystem::path& file,
                     uint16_t permissions,
                     const std::string& owner,
                     uint16_t owner_id,
                     const std::string& group,
                     uint16_t group_id,
                     PartsCompressionParameters compression_parameters) :
        BaseEntry(root, file, permissions, owner, owner_id, group, group_id),
        m_compressionType(compression_parameters.m_fileCompression),
        m_uncompressedSize(0),
        m_compressedSize(0),
        m_offset(0),
        m_compressionParameters(compression_parameters)
    {}

    virtual std::unique_ptr<Compressor> createCompressor();

    bool checkHashMatch(const std::filesystem::path& path);

    bool checkExisting(const std::filesystem::path& dest_root);


protected:
    CompressionType m_compressionType;
    Hash m_uncompressedHash;
    uint64_t m_uncompressedSize;
    uint64_t m_compressedSize;
    uint64_t m_offset;

    // In case of compression
    PartsCompressionParameters m_compressionParameters;
};

}
#endif // REGULARFILEENTRY_H
