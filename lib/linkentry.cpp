#include "linkentry.h"

#include "packager.h"
#include "internal_definitions.h"
#include "logger_internal.h"

#include <boost/filesystem.hpp>

#include <fmt/format.h>
#include <fmt/time.h>

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
LinkEntry::LinkEntry(InputBuffer& buffer, const std::vector<std::string>& owners, const std::vector<std::string>& groups) :
    BaseEntry(buffer, owners, groups)
{
    uint8_t tmp = 0;
    Packager::pop_front(buffer, tmp);
    m_absolute = tmp;

    Packager::pop_front(buffer, m_destination);
}

//==========================================================================================================================================
void LinkEntry::append(std::vector<uint8_t>& buffer) const
{
    buffer.push_back(static_cast<uint8_t>(EntryTypes::Link));
    BaseEntry::append(buffer);
    Packager::append(buffer, static_cast<uint8_t>(m_absolute));
    Packager::append(buffer, m_destination);
}

//==========================================================================================================================================
void LinkEntry::compressEntry(const boost::filesystem::path& root, ContentWriteBackend& backend)
{
    // This function is empty, since there is no file to be compressed
    LOG_TRACE("Compressing link: {}", m_file.string());
}

//==========================================================================================================================================
void LinkEntry::extractEntry(const boost::filesystem::path& dest_root, ContentReadBackend& backend, bool cont)
{
    LOG_TRACE("Create link:  {}", m_file.string());

    // Symlink already created, since we continues the update this won't cause problem.
    if (cont && boost::filesystem::is_symlink(dest_root / m_file) && boost::filesystem::read_symlink(dest_root / m_file) == m_destination)
        return;

    boost::system::error_code ec;
    boost::filesystem::create_symlink(m_destination, dest_root / m_file, ec);
    if (ec)
        throw PartsException("Cannot create symbolic link: '" + (dest_root / m_file).string() + "' to '" + m_destination.string() +"'");
}

//==========================================================================================================================================
void LinkEntry::updateEntry(const BaseEntry* old_entry,
                            const boost::filesystem::path& old_root,
                            const boost::filesystem::path& dest_root,
                            ContentReadBackend& backend,
                            bool cont)
{
    // Do the same in this case too, since there is no data extraction
    extractEntry(dest_root, backend, cont);
}

//==========================================================================================================================================
std::string LinkEntry::listEntry(size_t user_width, size_t size_width, std::tm* t) const
{
    auto group_user = fmt::format("{:<{}}", owner() + " " + group(), user_width);
    auto size_str = fmt::format("{:>{}}", 0, size_width);
    //2018-02-28 15:34
    return fmt::format("l{}   1 {} {} {:%m-%d-%Y %R} {} -> {}",
                       permissionsToString(),
                       group_user,
                       size_str,
                       *t,
                       file().string(),
                       m_destination.string());
}

//==========================================================================================================================================
std::string LinkEntry::toString() const
{
    return fmt::format("{} --- type: link, target: {}", BaseEntry::toString(), m_destination.string());
}
