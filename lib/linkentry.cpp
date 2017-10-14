#include "linkentry.h"

#include "packager.h"
#include "internal_definitions.h"

using namespace parts;


//==========================================================================================================================================
LinkEntry::LinkEntry(const boost::filesystem::path& file,
                     uint16_t permissions,
                     const std::string& owner,
                     uint16_t owner_id,
                     const std::string& group,
                     uint16_t group_id,
                     const boost::filesystem::path& destination,
                     bool absolute) :
    BaseEntry(file,
              permissions,
              owner,
              owner_id,
              group,
              group_id),
    m_destination(destination),
    m_absolute(absolute)
{
}

//==========================================================================================================================================
void LinkEntry::append(std::deque<uint8_t>& buffer) const
{
    buffer.push_back(static_cast<uint8_t>(EntryTypes::Link));
    BaseEntry::append(buffer);
    Packager::append(buffer, static_cast<uint8_t>(m_absolute));
    Packager::append(buffer, m_destination);
}

//==========================================================================================================================================
void LinkEntry::compressEntry(Compressor& compressor, ContentWriteBackend& backend)
{
    // This function is empty, since there is no file to be compressed
}

