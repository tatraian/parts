#include "directoryentry.h"

#include "parts_definitions.h"
#include "internal_definitions.h"
#include "logger_internal.h"

#include <boost/filesystem.hpp>

#include <fmt/format.h>
#include <fmt/time.h>

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
DirectoryEntry::DirectoryEntry(InputBuffer& buffer,
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
void DirectoryEntry::compressEntry(const boost::filesystem::path& root, ContentWriteBackend& backend)
{
    // Do nothing
    LOG_TRACE("Compressing dir:  {}", m_file.string());
}

//==========================================================================================================================================
void DirectoryEntry::extractEntry(const boost::filesystem::path& dest_root, ContentReadBackend& backend)
{
    LOG_TRACE("Create dir:   {}", m_file.string());
    // Do not decompression, but creates the directory with the correct rights
    boost::system::error_code ec;
    boost::filesystem::create_directories(dest_root / m_file, ec);
    if (ec)
    {
        throw PartsException("Cannot create directory: " + (dest_root / m_file).string());
    }

    setMetadata(dest_root);
}

//==========================================================================================================================================
void DirectoryEntry::updateEntry(const BaseEntry* old_entry,
                                 const boost::filesystem::path& old_root,
                                 const boost::filesystem::path& dest_root,
                                 ContentReadBackend& backend,
                                 bool checkExisting)
{
    if (checkExisting && boost::filesystem::exists( dest_root / m_file )) {
        LOG_TRACE("Directory already exists, don't create:    {}", (dest_root / m_file).string());
        setMetadata(dest_root);
        return;
    }

    // Do the same as in case of extract, (that is good if the old entry is file or link too)
    extractEntry(dest_root, backend);
}

//==========================================================================================================================================
std::string DirectoryEntry::listEntry(size_t user_width, size_t size_width, std::tm* t) const
{
    auto group_user = fmt::format("{:<{}}", owner() + " " + group(), user_width);
    auto size_str = fmt::format("{:>{}}", 0, size_width);
    //2018-02-28 15:34
    return fmt::format("d{}   1 {} {} {:%m-%d-%Y %R} {}/", permissionsToString(), group_user, size_str, *t, file().string());
}

//==========================================================================================================================================
std::string DirectoryEntry::toString() const
{
    return fmt::format("{} --- type: directory", BaseEntry::toString());
}
