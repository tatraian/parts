#include "tableofcontents.h"
#include "directoryentry.h"
#include "regularfileentry.h"
#include "linkentry.h"

#include <boost/filesystem.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

using namespace parts;

const std::string DEFAULT_OWNER = "__PARTS_DEFAULT_OWNER__";
const std::string DEFAULT_GROUP = "__PARTS_DEFAULT_GROUP__";


//==========================================================================================================================================
TableOfContents::TableOfContents(const boost::filesystem::path& root, const PartsCompressionParameters& parameters) :
    m_parameters(parameters)
{
    m_owners.push_back(DEFAULT_OWNER);
    m_groups.push_back(DEFAULT_GROUP);

    if (!boost::filesystem::exists(root))
        throw PartsException("Root directory doesn't exist: " + root.string());

    if (boost::filesystem::is_regular_file(root) || boost::filesystem::is_symlink(root))
    {
        add(root.parent_path(), root);
        return;
    }

    boost::filesystem::recursive_directory_iterator dir(root), end;
    for (;dir != end;++dir)
    {
        add(root, dir->path());
        if (boost::filesystem::is_symlink(dir->path()))
            dir.no_push();
    }
}

//==========================================================================================================================================
void TableOfContents::add(const boost::filesystem::path& root, const boost::filesystem::path& file)
{
    boost::filesystem::path filename = boost::filesystem::relative(file, root);

    // getting stat
    struct stat file_stat;
    lstat(file.c_str(), &file_stat);

    uint16_t permissions = getPermissions(&file_stat);
    uint16_t owner_id    = getOwnerId(&file_stat);
    uint16_t group_id    = getGroupId(&file_stat);

    std::shared_ptr<BaseEntry> entry;
    if (boost::filesystem::is_regular_file(file))
    {
        Hash hash(m_parameters.m_hashType, file);
        entry.reset(new RegularFileEntry(filename,
                                         permissions,
                                         m_owners[owner_id],
                                         owner_id,
                                         m_groups[group_id],
                                         group_id,
                                         hash,
                                         boost::filesystem::file_size(file)));
    } else if (boost::filesystem::is_directory(file))
    {
        entry.reset(new DirectoryEntry(filename,
                                       permissions,
                                       m_owners[owner_id],
                                       owner_id,
                                       m_groups[group_id],
                                       group_id));
    } else if (boost::filesystem::is_symlink(file))
    {
        boost::filesystem::path target = boost::filesystem::read_symlink(file);
        entry.reset(new LinkEntry(filename,
                                  permissions,
                                  m_owners[owner_id],
                                  owner_id,
                                  m_groups[group_id],
                                  group_id,
                                  target,
                                  false));
    }

    m_files[entry->file()] = entry;
}

//==========================================================================================================================================
uint16_t TableOfContents::getOwnerId(const struct stat *file_stat)
{
    // if owner is not saved, we return with the default user
    if (!m_parameters.m_saveOwners)
        return 0;

    struct passwd* pw = getpwuid(file_stat->st_uid);
    std::string owner(pw->pw_name);
    return findOrInsert(owner, m_owners);
}

//==========================================================================================================================================
uint16_t TableOfContents::getGroupId(const struct stat *file_stat)
{
    // if owner is not saved, we return with the default user
    if (!m_parameters.m_saveOwners)
        return 0;

    struct group*  gr = getgrgid(file_stat->st_gid);
    std::string group(gr->gr_name);
    return findOrInsert(group, m_groups);
}

//==========================================================================================================================================
uint16_t TableOfContents::getPermissions(const struct stat *file_stat)
{
    return static_cast<uint16_t>(file_stat->st_mode & (S_IRWXU|S_IRWXO|S_IRWXG));
}

//==========================================================================================================================================
uint16_t TableOfContents::findOrInsert(const std::string& name, std::vector<std::string>& table)
{
    auto it = std::find(table.begin(), table.end(), name);
    if (it == table.end())
    {
        table.push_back(name);
        return table.size() - 1;
    }

    return it - table.begin();
}
