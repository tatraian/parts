#include "baseentry.h"

#include "packager.h"

using namespace parts;


//==========================================================================================================================================
std::vector<uint8_t> BaseEntry::getRaw() const
{
    // Entry Name: 32 bytes, + size 2 bytes + owner 1 byte + group 1 byte + permissios 2 bytes
    const size_t DEFAULT_SIZE = 38;
    std::vector<uint8_t> result;
    result.reserve(DEFAULT_SIZE);

    Packager::append(result, m_file);
    Packager::append(result, m_permissions);
    Packager::append(result, m_ownerId);
    Packager::append(result, m_groupId);

    return result;
}

