#include "directoryentry.h"

#include "internal_definitions.h"

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
