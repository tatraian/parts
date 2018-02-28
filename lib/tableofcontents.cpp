#include "tableofcontents.h"
#include "directoryentry.h"
#include "regularfileentry.h"
#include "linkentry.h"
#include "packager.h"
#include "decompressorfactory.h"
#include "internal_definitions.h"
#include "logger.h"

#include <boost/filesystem.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#include <chrono>

using namespace parts;

const std::string TableOfContents::DEFAULT_OWNER = "__PARTS_DEFAULT_OWNER__";
const std::string TableOfContents::DEFAULT_GROUP = "__PARTS_DEFAULT_GROUP__";

//==========================================================================================================================================
TableOfContents::TableOfContents(const boost::filesystem::path& source, const PartsCompressionParameters& parameters) :
    m_parameters(parameters)
{
    LOG_TRACE("Source: {}", source.string());
    m_owners.push_back(DEFAULT_OWNER);
    m_groups.push_back(DEFAULT_GROUP);

    if (!boost::filesystem::exists(source))
        throw PartsException("Source doesn't exist: " + source.string());

    add(source.parent_path(), source);
    if (boost::filesystem::is_regular_file(source) || boost::filesystem::is_symlink(source))
        return;

    auto start_time = std::chrono::system_clock::now();
    LOG_TRACE("Creating TOC");
    boost::filesystem::recursive_directory_iterator dir(source), end;
    for (;dir != end;++dir)
    {
        add(source.parent_path(), dir->path());
        if (boost::filesystem::is_symlink(dir->path()))
            dir.no_push();
    }
    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end_time-start_time;
    LOG_DEBUG("TOC creation time: {} s", diff.count());
}

//==========================================================================================================================================
TableOfContents::TableOfContents(ContentReadBackend& backend, size_t toc_size, const PartsCompressionParameters& parameters) :
    m_parameters(parameters)
{
    std::vector<uint8_t> compressed_toc(toc_size, 0);
    backend.read(compressed_toc);

    LOG_DEBUG("Decompressing TOC");
    auto decompressor = DecompressorFactory::createDecompressor(m_parameters.m_tocCompression);
    InputBuffer uncompressed_toc = decompressor->extractBuffer(compressed_toc);

    unpackNames(uncompressed_toc, m_owners);
    unpackNames(uncompressed_toc, m_groups);

    while (!uncompressed_toc.empty())
    {
        uint8_t tmp = 0;
        Packager::pop_front(uncompressed_toc, tmp);
        EntryTypes type = static_cast<EntryTypes>(tmp);
        std::shared_ptr<BaseEntry> entry;
        switch (type) {
        case EntryTypes::RegularFile:
            entry.reset(new RegularFileEntry(uncompressed_toc, m_owners, m_groups, m_parameters.m_hashType));
            break;
        case EntryTypes::Directory:
            entry.reset(new DirectoryEntry(uncompressed_toc, m_owners, m_groups));
            break;
        case EntryTypes::Link:
            entry.reset(new LinkEntry(uncompressed_toc, m_owners, m_groups));
            break;
        default:
            throw PartsException("Unknown node type: " + std::to_string(tmp));
        }

        LOG_DEBUG("TOC entry: {}", entry->toString());
        m_files[entry->file()] = entry;
    }
}

//==========================================================================================================================================
std::vector<uint8_t> TableOfContents::getRaw() const
{
    std::vector<uint8_t> result;
    result.reserve(1024*1024);

    packNames(result, m_owners);
    packNames(result, m_groups);

    for(const auto& entry : m_files){
        entry.second->append(result);
    }

    return result;
}

//==========================================================================================================================================
void TableOfContents::shiftOffsets(uint64_t& data_start)
{
    for(auto entry : m_files) {
        auto rf = dynamic_cast<RegularFileEntry*>(entry.second.get());
        if (rf != nullptr)
            rf->shiftOffset(data_start);
    }
}

//==========================================================================================================================================
std::shared_ptr<BaseEntry> TableOfContents::find(const boost::filesystem::path& file)
{
    auto it = m_files.find(file);
    if (it == m_files.end())
        return std::shared_ptr<BaseEntry>();

    return it->second;
}

//==========================================================================================================================================
void TableOfContents::add(const boost::filesystem::path& root, const boost::filesystem::path& file)
{
    boost::filesystem::path filename = file.lexically_relative(root);

    // getting stat
    struct stat file_stat;
    lstat(file.c_str(), &file_stat);

    uint16_t permissions = getPermissions(&file_stat);
    uint16_t owner_id    = getOwnerId(&file_stat);
    uint16_t group_id    = getGroupId(&file_stat);

    std::shared_ptr<BaseEntry> entry;
    // must check this first, because depending of the target of the link is_regular_file and is_directory are also true...
    if (boost::filesystem::is_symlink(file)) {
        boost::filesystem::path target = boost::filesystem::read_symlink(file);

        entry.reset(new LinkEntry(filename,
                                  permissions,
                                  m_owners[owner_id],
                                  owner_id,
                                  m_groups[group_id],
                                  group_id,
                                  // order is important otherwise fileInsideRoot will die with relative paths
                                  target.is_absolute() && fileInsideRoot(root, target) ? target.lexically_relative(root) : target,
                                  target.is_absolute()));
    } else if (boost::filesystem::is_directory(file)) {
        entry.reset(new DirectoryEntry(filename,
                                       permissions,
                                       m_owners[owner_id],
                                       owner_id,
                                       m_groups[group_id],
                                       group_id));
    } else if (boost::filesystem::is_regular_file(file)) {
        Hash hash(m_parameters.m_hashType, file);
        entry.reset(new RegularFileEntry(filename,
                                         permissions,
                                         m_owners[owner_id],
                                         owner_id,
                                         m_groups[group_id],
                                         group_id,
                                         hash,
                                         boost::filesystem::file_size(file)));
    } else {
        // TODO log here!
        return;
    }
    LOG_DEBUG("Adding TOC entry: {}", entry->toString());

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
    return static_cast<uint16_t>(file_stat->st_mode & (S_IRWXU|S_IRWXO|S_IRWXG|S_ISUID|S_ISGID));
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

//==========================================================================================================================================
bool TableOfContents::fileInsideRoot(const boost::filesystem::path& root, const boost::filesystem::path& file)
{
    auto real_root = boost::filesystem::canonical(root);
    auto real_file = boost::filesystem::canonical(file);

    return real_file.string().find(real_root.string()) == 0;
}

//==========================================================================================================================================
void TableOfContents::packNames(std::vector<uint8_t>& buffer, const std::vector<std::string>& names) const
{
    Packager::append(buffer, static_cast<uint16_t>(names.size()));
    for(const std::string& name : names) {
        Packager::append<uint16_t>(buffer, name);
    }
}

//==========================================================================================================================================
void TableOfContents::unpackNames(InputBuffer& buffer, std::vector<std::string>& names)
{
    uint16_t size = 0;
    Packager::pop_front(buffer, size);
    names.resize(size);
    for (size_t tmp = 0; tmp != size; ++tmp) {
        Packager::pop_front<uint16_t>(buffer, names[tmp]);
    }
}

//==========================================================================================================================================
TableOfContents::TableOfContents(const PartsCompressionParameters& params) : m_parameters(params)
{
    // Unit test only constructor, It does nothing
}
