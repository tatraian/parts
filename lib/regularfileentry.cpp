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
RegularFileEntry::RegularFileEntry(const boost::filesystem::path& root,
                                   const boost::filesystem::path& file,
                                   std::vector<std::string>& owners,
                                   std::vector<std::string>& groups,
                                   bool save_owner,
                                   PartsCompressionParameters compression_parameters) noexcept :
    BaseEntry(root, file, owners, groups, save_owner),
    m_compressionType(compression_parameters.m_fileCompression),
    m_uncompressedSize(0),
    m_compressedSize(0),
    m_offset(0),
    m_compressionParameters(compression_parameters)
{
    if (!m_valid)
        return;

    try {
        m_uncompressedHash = Hash(m_compressionParameters.m_hashType, root/file);
        m_uncompressedSize = boost::filesystem::file_size(root/file);
    } catch(const std::exception&) {
        LOG_ERROR("Cannot get file size: {}", (root/file).string());
        m_valid = false;
    }
}

//==========================================================================================================================================
RegularFileEntry::RegularFileEntry(InputBuffer& buffer,
                                   const std::vector<std::string>& owners,
                                   const std::vector<std::string>& groups,
                                   HashType hash_type) noexcept :
    BaseEntry(buffer, owners, groups),
    m_uncompressedHash(hash_type, buffer)
{
    if (!m_valid && !m_uncompressedHash.isValid())
        return;

    try {
        uint8_t tmp;
        Packager::pop_front(buffer, tmp);
        m_compressionType = static_cast<CompressionType>(tmp);
        Packager::pop_front(buffer, m_uncompressedSize);
        Packager::pop_front(buffer, m_compressedSize);
        Packager::pop_front(buffer, m_offset);
    } catch(const std::exception&) {
        LOG_ERROR("Cannot get read regular file stream");
        m_valid = false;
    }
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
void RegularFileEntry::compressEntry(ContentWriteBackend& backend)
{
    LOG_TRACE("Compressing file: {}", m_file.string());

    auto compressor = createCompressor();

    m_offset = backend.getPosition();
    m_compressedSize = compressor->compressFile(m_root / m_file, backend);
}

//==========================================================================================================================================
void RegularFileEntry::extractEntry(const boost::filesystem::path& dest_root, ContentReadBackend& backend, bool cont)
{
    LOG_TRACE("Extract file: {}", m_file.string());
    if (cont && checkExisting(dest_root)) {
        setMetadata(dest_root);
        return;
    }

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
    auto file_old_entry = dynamic_cast<const RegularFileEntry*>(old_entry);
    if (file_old_entry == nullptr || file_old_entry->m_uncompressedHash != m_uncompressedHash) {
        extractEntry(dest_root, backend, cont);
        return;
    }

    // in case of copy we also have to check the possibility of continue:
    LOG_TRACE("Copy file:    {}", m_file.string());
    if (cont && checkExisting(dest_root)) {
        setMetadata(dest_root);
        return;
    }

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
bool RegularFileEntry::checkHashMatch(const boost::filesystem::path& path)
{
    Hash existing_hash(m_uncompressedHash.type(), path);
    LOG_DEBUG("Hashes: {}\n        {}", existing_hash.hashString(), m_uncompressedHash.hashString());
    return existing_hash == m_uncompressedHash;
}

//==========================================================================================================================================
bool RegularFileEntry::checkExisting(const boost::filesystem::path& dest_root)
{
    if (boost::filesystem::is_regular_file(dest_root / m_file)) {
        if (checkHashMatch(dest_root / m_file)) {
            LOG_TRACE("File already exsists: {}", m_file.string());
            return true;
        }
        LOG_TRACE("Erasing half extracted entry: {}", m_file.string());
        boost::filesystem::remove(dest_root / m_file);
    }

    return false;
}

//==========================================================================================================================================
bool RegularFileEntry::extractToMc(const boost::filesystem::path& dest_file, ContentReadBackend& backend)
{
    auto decompressor = DecompressorFactory::createDecompressor(m_compressionType);
    decompressor->extractFile(dest_file, backend, m_offset, m_compressedSize);
    return true;
}
