#include "baseentry.h"

#include "packager.h"

using namespace parts;


//==========================================================================================================================================
BaseEntry::BaseEntry(std::deque<uint8_t>& buffer, const std::vector<std::string>& owners, const std::vector<std::string>& groups)
{
    Packager::pop_front(buffer, m_file);
    Packager::pop_front(buffer, m_permissions);
    Packager::pop_front(buffer, m_ownerId);
    Packager::pop_front(buffer, m_groupId);

    m_owner = owners[m_ownerId];
    m_group = groups[m_groupId];
}

//==========================================================================================================================================
void BaseEntry::append(std::vector<uint8_t>& buffer) const
{
    Packager::append(buffer, m_file);
    Packager::append(buffer, m_permissions);
    Packager::append(buffer, m_ownerId);
    Packager::append(buffer, m_groupId);
}
