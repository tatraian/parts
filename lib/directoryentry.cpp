#include "directoryentry.h"

#include "parts_definitions.h"
#include "internal_definitions.h"

#include <boost/filesystem.hpp>

#include <fmt/format.h>

using namespace parts;

//==========================================================================================================================================
DirectoryEntry::DirectoryEntry(const boost::filesystem::path& file,
                               uint16_t permissions,
                               const std::string& owner,
                               uint16_t owner_id,
                               const std::string& group,
                               uint16_t group_id) :
    BaseEntry(file, permissions, owner, owner_id, group, group_id)
{
}

//==========================================================================================================================================
DirectoryEntry::DirectoryEntry(std::deque<uint8_t>& buffer,
                               const std::vector<std::string>& owners,
                               const std::vector<std::string>& groups) :
    BaseEntry(buffer, owners, groups)
{
}

//==========================================================================================================================================
void DirectoryEntry::append(std::vector<uint8_t>& buffer) const
{
    buffer.push_back(static_cast<uint8_t>(EntryTypes::Directory));
    BaseEntry::append(buffer);
    // There is no own data
}

//==========================================================================================================================================
void DirectoryEntry::compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend)
{
    // Do nothing
}

//==========================================================================================================================================
void DirectoryEntry::extractEntry(const boost::filesystem::path& dest_root, Decompressor& decompressor, ContentReadBackend& backend)
{
    // Do not decompression, but creates the directory with the correct rights
    if (!boost::filesystem::create_directory(dest_root / m_file))
        throw PartsException("Cannot create directory: " + (dest_root / m_file).string());

    setMetadata(dest_root);
}

//==========================================================================================================================================
std::string DirectoryEntry::toString() const
{
    return fmt::format("{} --- type: directory", BaseEntry::toString());
}
