#include "regularfileentry.h"

#include "internal_definitions.h"
#include "packager.h"

#include <boost/filesystem.hpp>
#include <fmt/format.h>

using namespace parts;


//==========================================================================================================================================
RegularFileEntry::RegularFileEntry(const boost::filesystem::path& file,
                                   uint16_t permissions,
                                   const std::string& owner,
                                   uint16_t owner_id,
                                   const std::string& group,
                                   uint16_t group_id,
                                   Hash uncompressed_hash,
                                   uint64_t uncompressed_size) :
    BaseEntry(file, permissions, owner, owner_id, group, group_id),
    m_uncompressedHash(uncompressed_hash),
    m_uncompressedSize(uncompressed_size),
    m_compressedSize(0),
    m_offset(0)
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
    Packager::append(buffer, m_uncompressedSize);
    Packager::append(buffer, m_compressedSize);
    Packager::append(buffer, m_offset);
}

//==========================================================================================================================================
void RegularFileEntry::compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend)
{
    m_offset = backend.getPosition();
    m_compressedSize = compressor.compressFile(root / m_file, backend);
}

//==========================================================================================================================================
void RegularFileEntry::extractEntry(const boost::filesystem::path& dest_root, Decompressor& decompressor, ContentReadBackend& backend)
{
    decompressor.extractFile(dest_root / m_file, backend, m_offset, m_compressedSize);

    setMetadata(dest_root);
}

//==========================================================================================================================================
void RegularFileEntry::updateEntry(const BaseEntry* old_entry,
                                   const boost::filesystem::path& old_root,
                                   const boost::filesystem::path& dest_root,
                                   Decompressor& decompressor,
                                   ContentReadBackend& backend)
{
    auto file_old_entry = dynamic_cast<const RegularFileEntry*>(old_entry);
    if (file_old_entry == nullptr || file_old_entry->m_uncompressedHash != m_uncompressedHash) {
        extractEntry(dest_root, decompressor, backend);
        return;
    }

    boost::filesystem::copy_file(old_root / old_entry->file(), dest_root / m_file);
    setMetadata(dest_root);
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
