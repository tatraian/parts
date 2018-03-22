#include "regularfileentry.h"

#include "internal_definitions.h"
#include "packager.h"
#include "logger_internal.h"
#include "compressorfactory.h"
#include "decompressorfactory.h"

#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <fmt/time.h>

using namespace parts;


//==========================================================================================================================================
RegularFileEntry::RegularFileEntry(const boost::filesystem::path& relfile,
                                   const boost::filesystem::path& file,
                                   uint16_t permissions,
                                   const std::string& owner,
                                   uint16_t owner_id,
                                   const std::string& group,
                                   uint16_t group_id,
                                   CompressionType compression_hint,
                                   const PartsCompressionParameters & compression_parameters,
                                   uint64_t compressed_size,
                                   uint64_t uncompressed_size,
                                   uint64_t offset) :
    BaseEntry(relfile, permissions, owner, owner_id, group, group_id),
    m_uncompressedHash(compression_parameters.m_hashType, file),
    m_uncompressedSize(uncompressed_size),
    m_compressionHint(compression_hint),
    m_compressionParameters(compression_parameters),
    m_compressedSize(compressed_size),
    m_offset(offset)
{
}

//==========================================================================================================================================
RegularFileEntry::RegularFileEntry(const boost::filesystem::path& relfile,
                                   const boost::filesystem::path& file,
                                   uint16_t permissions,
                                   const std::string& owner,
                                   uint16_t owner_id,
                                   const std::string& group,
                                   uint16_t group_id,
                                   const Hash & uncompressed_hash,
                                   CompressionType compression_hint,
                                   const PartsCompressionParameters & compression_parameters,
                                   uint64_t compressed_size,
                                   uint64_t uncompressed_size,
                                   uint64_t offset) :
    BaseEntry(relfile, permissions, owner, owner_id, group, group_id),
    m_uncompressedHash(uncompressed_hash),
    m_uncompressedSize(uncompressed_size),
    m_compressionHint(compression_hint),
    m_compressionParameters(compression_parameters),
    m_compressedSize(compressed_size),
    m_offset(offset)
{
}

//==========================================================================================================================================
RegularFileEntry::RegularFileEntry(InputBuffer& buffer,
                                   const std::vector<std::string>& owners,
                                   const std::vector<std::string>& groups,
                                   const PartsCompressionParameters & compression_parameters) :
    BaseEntry(buffer, owners, groups),
    m_uncompressedHash(compression_parameters.m_hashType, buffer),
    m_compressionHint(compression_parameters.m_fileCompression),
    m_compressionParameters(compression_parameters)
{
    Packager::pop_front(buffer, m_uncompressedSize);
    uint8_t l_compressionHint;
    Packager::pop_front(buffer, l_compressionHint);
    m_compressionHint = static_cast<CompressionType>(l_compressionHint);
    Packager::pop_front(buffer, m_compressedSize);
    Packager::pop_front(buffer, m_offset);
}

//==========================================================================================================================================
void RegularFileEntry::append(std::vector<uint8_t>& buffer) const
{
    buffer.push_back(static_cast<uint8_t>(EntryTypes::RegularFile));
    BaseEntry::append(buffer);
    Packager::append(buffer, m_uncompressedHash.hash());
    Packager::append(buffer, m_uncompressedSize);
    Packager::append(buffer, static_cast<uint8_t>(m_compressionHint));
    Packager::append(buffer, m_compressedSize);
    Packager::append(buffer, m_offset);
}

//==========================================================================================================================================
void RegularFileEntry::compressEntry(const boost::filesystem::path& root, ContentWriteBackend& backend)
{
    LOG_TRACE("Compressing file: {}", m_file.string());
    m_offset = backend.getPosition();
    auto compressor = CompressorFactory::createCompressor(m_compressionHint, m_compressionParameters);
    m_compressedSize = compressor->compressFile(root / m_file, backend);
}

//==========================================================================================================================================
void RegularFileEntry::extractEntry(const boost::filesystem::path& dest_root, ContentReadBackend& backend)
{
    LOG_TRACE("Extract file: {}", m_file.string());
    auto decompressor = DecompressorFactory::createDecompressor(m_compressionHint);
    decompressor->extractFile(dest_root / m_file, backend, m_offset, m_compressedSize, m_uncompressedSize);

    if (m_compressionParameters.m_compareHash) {
        LOG_TRACE("Checking hash for file: {}", m_file.string());
        Hash hash(m_uncompressedHash.type(), dest_root / m_file);
        if (hash != m_uncompressedHash) {
            throw PartsException("Invalid hash " + hash.hashString() + "<>" + m_uncompressedHash.hashString() + " for " + m_file.string());
        }
    }

    setMetadata(dest_root);
}

//==========================================================================================================================================
void RegularFileEntry::updateEntry(const BaseEntry* old_entry,
                                   const boost::filesystem::path& old_root,
                                   const boost::filesystem::path& dest_root,
                                   ContentReadBackend& backend,
                                   bool checkExisting)
{
    auto file_old_entry = dynamic_cast<const RegularFileEntry*>(old_entry);

    if (file_old_entry == nullptr || file_old_entry->m_uncompressedHash != m_uncompressedHash) {
        extractEntry(dest_root, backend);
        return;
    }

    if (checkExisting && boost::filesystem::exists( dest_root / m_file ) ) {
        LOG_TRACE("File already exists, don't copy:    {}", m_file.string());
        setMetadata(dest_root);
        return;
    }

    LOG_TRACE("Copy file:    {}", m_file.string());
    boost::filesystem::copy_file(old_root / old_entry->file(), dest_root / m_file);

    if (m_compressionParameters.m_compareHash) {
        LOG_TRACE("Checking hash for file: {}", m_file.string());
        Hash hash(m_uncompressedHash.type(), dest_root / m_file);
        if (hash != m_uncompressedHash) {
            throw PartsException("Invalid hash " + hash.hashString() + "<>" + m_uncompressedHash.hashString() + " for " + m_file.string());
        }
    }

    setMetadata(dest_root);
}

//==========================================================================================================================================
std::string RegularFileEntry::listEntry(size_t user_width, size_t size_width, std::tm* t) const
{
    auto group_user = fmt::format("{:<{}}", owner() + " " + group(), user_width);
    auto size_str = fmt::format("{:>{}}", m_uncompressedSize, size_width);
    //2018-02-28 15:34
    return fmt::format("-{}   1 {} {} {:%m-%d-%Y %R} {}", permissionsToString(), group_user, size_str, *t, file().string());
}

//==========================================================================================================================================
std::string RegularFileEntry::toString() const
{
    return fmt::format("{} --- type: file, offset: {}, ucs: {}, uch: {}, cs: {}",
                       BaseEntry::toString(),
                       m_offset,
                       m_uncompressedSize,
                       m_uncompressedHash.hashString(),
                       m_compressedSize);
}

//==========================================================================================================================================
bool RegularFileEntry::extractToMc(const boost::filesystem::path& dest_file, ContentReadBackend& backend)
{
    auto decompressor = DecompressorFactory::createDecompressor(m_compressionHint);
    decompressor->extractFile(dest_file, backend, m_offset, m_compressedSize, m_uncompressedSize);
    return true;
}
