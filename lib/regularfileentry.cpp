#include "regularfileentry.h"

#include "internal_definitions.h"
#include "packager.h"
#include "logger_internal.h"

#include "compressorfactory.h"
#include "decompressorfactory.h"

#include "plaincompressor.h"

#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <fmt/time.h>

using namespace parts;


//==========================================================================================================================================
RegularFileEntry::RegularFileEntry(const boost::filesystem::path& file,
                                   uint16_t permissions,
                                   const std::string& owner,
                                   uint16_t owner_id,
                                   const std::string& group,
                                   uint16_t group_id,
                                   PartsCompressionParameters compression_parameters) :
    BaseEntry(file, permissions, owner, owner_id, group, group_id),
    m_compressionType(compression_parameters.m_fileCompression),
    m_uncompressedSize(0),
    m_compressedSize(0),
    m_offset(0),
    m_compressionParameters(compression_parameters)
{
}

//==========================================================================================================================================
RegularFileEntry::RegularFileEntry(InputBuffer& buffer,
                                   const std::vector<std::string>& owners,
                                   const std::vector<std::string>& groups,
                                   HashType hash_type) :
    BaseEntry(buffer, owners, groups),
    m_uncompressedHash(hash_type, buffer)
{
    uint8_t tmp;
    Packager::pop_front(buffer, tmp);
    m_compressionType = static_cast<CompressionType>(tmp);
    Packager::pop_front(buffer, m_uncompressedSize);
    Packager::pop_front(buffer, m_compressedSize);
    Packager::pop_front(buffer, m_offset);
}

//==========================================================================================================================================
void RegularFileEntry::append(std::vector<uint8_t>& buffer) const
{
    buffer.push_back(static_cast<uint8_t>(EntryTypes::RegularFile));
    BaseEntry::append(buffer);
    Packager::append(buffer, m_uncompressedHash.hash());
    uint8_t tmp = static_cast<uint8_t>(m_compressionType);
    Packager::append(buffer, tmp);
    Packager::append(buffer, m_uncompressedSize);
    Packager::append(buffer, m_compressedSize);
    Packager::append(buffer, m_offset);
}

//==========================================================================================================================================
void RegularFileEntry::compressEntry(const boost::filesystem::path& root, ContentWriteBackend& backend)
{
    LOG_TRACE("Compressing file: {}", m_file.string());
    setHashAndSize(root / m_file);

    auto compressor = createCompressor();

    m_offset = backend.getPosition();
    m_compressedSize = compressor->compressFile(root / m_file, backend);
}

//==========================================================================================================================================
void RegularFileEntry::extractEntry(const boost::filesystem::path& dest_root, ContentReadBackend& backend)
{
    LOG_TRACE("Extract file: {}", m_file.string());
    auto decompressor = DecompressorFactory::createDecompressor(m_compressionType);
    decompressor->extractFile(dest_root / m_file, backend, m_offset, m_compressedSize);

    setMetadata(dest_root);
}

//==========================================================================================================================================
void RegularFileEntry::updateEntry(const BaseEntry* old_entry,
                                   const boost::filesystem::path& old_root,
                                   const boost::filesystem::path& dest_root,
                                   ContentReadBackend& backend,
                                   bool cont)
{
    // if we continue the extracting
    if (cont && boost::filesystem::is_regular_file(dest_root / m_file)) {
        Hash existing_hash(m_uncompressedHash.type(), dest_root / m_file);
        LOG_DEBUG("Hashes: {}\n        {}", existing_hash.hashString(), m_uncompressedHash.hashString());
        if (existing_hash == m_uncompressedHash) {
            LOG_TRACE("File already exists: {}", m_file.string());
            setMetadata(dest_root);
            return;
        }
        LOG_TRACE("Erasing half extracted entry: {}", m_file.string());
        boost::filesystem::remove(dest_root / m_file);
    }

    auto file_old_entry = dynamic_cast<const RegularFileEntry*>(old_entry);
    if (file_old_entry == nullptr || file_old_entry->m_uncompressedHash != m_uncompressedHash) {
        extractEntry(dest_root, backend);
        return;
    }

    LOG_TRACE("Copy file:    {}", m_file.string());
    boost::filesystem::copy_file(old_root / old_entry->file(), dest_root / m_file);
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
    return fmt::format("{} --- type: file, compression: {}, offset: {}, ucs: {}, uch: {}, cs: {}",
                       BaseEntry::toString(),
                       to_string(m_compressionType),
                       m_offset,
                       m_uncompressedSize,
                       m_uncompressedHash.hashString(),
                       m_compressedSize);
}

//==========================================================================================================================================
void RegularFileEntry::setHashAndSize(const boost::filesystem::path& path)
{
    m_uncompressedHash = Hash(m_compressionParameters.m_hashType, path);
    m_uncompressedSize = boost::filesystem::file_size(path);
}

//==========================================================================================================================================
std::unique_ptr<Compressor> RegularFileEntry::createCompressor()
{

    if (m_uncompressedSize <= 100) {
        m_compressionType = CompressionType::None;
        return std::unique_ptr<Compressor>(new PlainCompressor());
    } else {
        m_compressionType = m_compressionParameters.m_fileCompression;
        return CompressorFactory::createCompressor(m_compressionParameters.m_fileCompression,  m_compressionParameters);
    }
}

//==========================================================================================================================================
bool RegularFileEntry::extractToMc(const boost::filesystem::path& dest_file, ContentReadBackend& backend)
{
    auto decompressor = DecompressorFactory::createDecompressor(m_compressionType);
    decompressor->extractFile(dest_file, backend, m_offset, m_compressedSize);
    return true;
}
