#include "regularfileentry.h"

#include "internal_definitions.h"
#include "packager.h"

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
void RegularFileEntry::append(std::deque<uint8_t>& buffer) const
{
    buffer.push_back(static_cast<uint8_t>(EntryTypes::RegularFile));
    BaseEntry::append(buffer);
    Packager::append(buffer, m_uncompressedHash.hash());
    Packager::append(buffer, m_uncompressedSize);
    Packager::append(buffer, m_compressedSize);
    Packager::append(buffer, m_offset);
}

//==========================================================================================================================================
void RegularFileEntry::compressEntry(Compressor& compressor, ContentWriteBackend& backend)
{
    m_offset = backend.getPosition();
    m_compressedSize = compressor.compressFile(m_file, backend);
}
