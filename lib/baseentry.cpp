#include "baseentry.h"

#include "packager.h"

#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

using namespace parts;


//==========================================================================================================================================
BaseEntry::BaseEntry(std::deque<uint8_t>& buffer, const std::vector<std::string>& owners, const std::vector<std::string>& groups)
{
    Packager::pop_front(buffer, m_file);
    Packager::pop_front(buffer, m_permissions);
    Packager::pop_front(buffer, m_ownerId);
    Packager::pop_front(buffer, m_groupId);

    if (owners.size() <= m_ownerId)
        throw PartsException("Unknown owner id, during processing file: " + m_file.string());
    m_owner = owners[m_ownerId];
    if (groups.size() <= m_groupId)
        throw PartsException("Unknown group id, during processing file: " + m_file.string());
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

//==========================================================================================================================================
void BaseEntry::setMetadata(const boost::filesystem::path& dest_root)
{
    if (m_ownerId != 0) {
        struct passwd* pw = getpwname(m_owner.c_str());
        struct ::group* gr = getgrnam(m_group.c_str());

        int result = chown((dest_root / m_file).c_str(), pw->pw_uid, gr->gr_gid);
        if (result != 0)
            throw PartsException("Cannot change owner/group of file: " + (dest_root / m_file).string());
    }

    int result = chmod((dest_root / m_file).c_str(), m_permissions);
    if (result != 0)
        throw PartsException("Cannot change permissions of file: " + (dest_root / m_file).string());
}
