#include "baseentry.h"

#include "packager.h"

#include <sstream>
#include <algorithm>

#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

#include "logger_internal.h"

using namespace parts;

//==========================================================================================================================================
BaseEntry::BaseEntry(const std::filesystem::path& root,
                     const std::filesystem::path& file,
                     std::vector<std::string>& owners,
                     std::vector<std::string>& groups,
                     bool save_owner) noexcept :
    m_valid(false),
    m_root(root),
    m_file(file)
{
    if (!std::filesystem::exists(root/file)) {
        LOG_ERROR("Path does not exists: {}", (root/file).string());
        return;
    }

    // getting stat
    struct stat file_stat;
    lstat((root / file).string().c_str(), &file_stat);

    m_permissions = getPermissions(&file_stat);
    m_ownerId = getOwnerId(&file_stat, owners, save_owner);
    m_owner = owners[m_ownerId];
    m_groupId = getGroupId(&file_stat, groups, save_owner);
    m_group = groups[m_groupId];
    m_valid = true;
}


//==========================================================================================================================================
BaseEntry::BaseEntry(InputBuffer& buffer, const std::vector<std::string>& owners, const std::vector<std::string>& groups) noexcept :
    m_valid(false)
{
    try {
        Packager::pop_front(buffer, m_file);
        Packager::pop_front(buffer, m_permissions);
        Packager::pop_front(buffer, m_ownerId);
        Packager::pop_front(buffer, m_groupId);
    } catch (const std::exception& e) {
       LOG_ERROR("Invalid binary data during parsing entry");
       return;
    }
    if (owners.size() <= m_ownerId) {
        LOG_ERROR("Unknown owner id, during processing file: {}" ,m_file.string());
        return;
    }
    m_owner = owners[m_ownerId];
    if (groups.size() <= m_groupId) {
        LOG_ERROR("Unknown group id, during processing file: {}",m_file.string());
        return;
    }
    m_group = groups[m_groupId];
    m_valid = true;
}

//==========================================================================================================================================
BaseEntry::~BaseEntry() noexcept
{
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
std::string BaseEntry::toString() const
{
    return fmt::format("{}: ({:o}, {}:{})", m_file.string(), m_permissions, m_owner, m_group);
}

//==========================================================================================================================================
void BaseEntry::setMetadata(const std::filesystem::path& dest_root)
{
    if (m_ownerId != 0) {
        struct passwd* pw = getpwnam(m_owner.c_str());
        struct ::group* gr = getgrnam(m_group.c_str());

        int result = chown((dest_root / m_file).c_str(), pw->pw_uid, gr->gr_gid);
        if (result != 0) {
            LOG_WARNING("Cannot change owner/group of file: {}", (dest_root / m_file).string());
            //throw PartsException("Cannot change owner/group of file: " + (dest_root / m_file).string());
        }
    }

    int result = chmod((dest_root / m_file).c_str(), m_permissions);
    if (result != 0) {
        LOG_WARNING("Cannot change permissions of file: {}", (dest_root / m_file).string());
        //throw PartsException("Cannot change permissions of file: " + (dest_root / m_file).string());
    }
}

//==========================================================================================================================================
std::string BaseEntry::permissionsToString() const
{
    std::stringstream output;

    output << ((m_permissions & S_IRUSR) ? "r" : "-");
    output << ((m_permissions & S_IWUSR) ? "w" : "-");
    output << ((m_permissions & S_IXUSR) ? "x" : "-");

    output << ((m_permissions & S_IRGRP) ? "r" : "-");
    output << ((m_permissions & S_IWGRP) ? "w" : "-");
    output << ((m_permissions & S_IXGRP) ? "x" : "-");

    output << ((m_permissions & S_IROTH) ? "r" : "-");
    output << ((m_permissions & S_IWOTH) ? "w" : "-");
    output << ((m_permissions & S_IXOTH) ? "x" : "-");

    return output.str();
}

//==========================================================================================================================================
uint16_t BaseEntry::getOwnerId(const struct stat* file_stat, std::vector<std::string>& owners, bool save)
{
    // if owner is not saved, we return with the default user
    if (!save)
        return 0;

    struct passwd* pw = getpwuid(file_stat->st_uid);
    std::string owner(pw->pw_name);
    return findOrInsert(owner, owners);
}

//==========================================================================================================================================
uint16_t BaseEntry::getGroupId(const struct stat* file_stat, std::vector<std::string>& groups, bool save)
{
    // if owner is not saved, we return with the default user
    if (!save)
        return 0;

    struct group*  gr = getgrgid(file_stat->st_gid);
    std::string group(gr->gr_name);
    return findOrInsert(group, groups);
}

//==========================================================================================================================================
uint16_t BaseEntry::getPermissions(const struct stat* file_stat)
{

    return static_cast<uint16_t>(file_stat->st_mode & (S_IRWXU|S_IRWXO|S_IRWXG|S_ISUID|S_ISGID));
}

//==========================================================================================================================================
uint16_t BaseEntry::findOrInsert(const std::string& name, std::vector<std::string>& table)
{
    auto it = std::find(table.begin(), table.end(), name);
    if (it == table.end())
    {
        table.push_back(name);
        return table.size() - 1;
    }

    return it - table.begin();
}

