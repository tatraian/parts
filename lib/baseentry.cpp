#include "baseentry.h"

#include "packager.h"

using namespace parts;


//==========================================================================================================================================
void BaseEntry::append(std::deque<uint8_t>& buffer) const
{
    Packager::append(buffer, m_file);
    Packager::append(buffer, m_permissions);
    Packager::append(buffer, m_ownerId);
    Packager::append(buffer, m_groupId);
}
