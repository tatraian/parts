#include "linkentry.h"

#include "packager.h"
#include "internal_definitions.h"
#include "logger_internal.h"

#include <filesystem>

#include <fmt/format.h>
#include <fmt/time.h>

using namespace parts;


//==========================================================================================================================================
LinkEntry::LinkEntry(const std::filesystem::path& root,
                     const std::filesystem::path& file,
                     std::vector<std::string>& owners,
                     std::vector<std::string>& groups,
                     bool save_owner) noexcept:
    BaseEntry(root, file, owners, groups, save_owner)
{
    try {
        std::filesystem::path target = std::filesystem::read_symlink(root/file);

        m_destination =target;
    } catch (const std::exception& e) {
        LOG_ERROR("Cannot read symbolic link target: {}", (root/file).string());
        m_valid = false;
    }
}

//==========================================================================================================================================
LinkEntry::LinkEntry(InputBuffer& buffer, const std::vector<std::string>& owners, const std::vector<std::string>& groups) noexcept:
    BaseEntry(buffer, owners, groups)
{
    if (!m_valid)
        return;

    try {
        Packager::pop_front(buffer, m_destination);
    } catch(const std::exception& e) {
        LOG_ERROR("Cannot read data from stream");
        m_valid = false;
    }
}

//==========================================================================================================================================
void LinkEntry::append(std::vector<uint8_t>& buffer) const
{
    buffer.push_back(static_cast<uint8_t>(EntryTypes::Link));
    BaseEntry::append(buffer);
    Packager::append(buffer, m_destination);
}

//==========================================================================================================================================
void LinkEntry::compressEntry(ContentWriteBackend& backend)
{
    // This function is empty, since there is no file to be compressed
    LOG_TRACE("Compressing link: {}", m_file.string());
}

//==========================================================================================================================================
void LinkEntry::extractEntry(const std::filesystem::path& dest_root, ContentReadBackend& backend, bool cont)
{
    LOG_TRACE("Create link:  {}", m_file.string());

    // Symlink already created, since we continues the update this won't cause problem.
    if (cont && std::filesystem::is_symlink(dest_root / m_file) && std::filesystem::read_symlink(dest_root / m_file) == m_destination)
        return;

    std::error_code ec;
    std::filesystem::create_symlink(m_destination, dest_root / m_file, ec);
    if (ec)
        throw PartsException("Cannot create symbolic link: '" + (dest_root / m_file).string() + "' to '" + m_destination.string() +"'");
}

//==========================================================================================================================================
void LinkEntry::updateEntry(const BaseEntry* old_entry,
                            const std::filesystem::path& old_root,
                            const std::filesystem::path& dest_root,
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

//==========================================================================================================================================
bool LinkEntry::fileInsideRoot(const std::filesystem::path& root, const std::filesystem::path& file)
{
    auto real_root = std::filesystem::canonical(root);
    auto real_file = std::filesystem::canonical(file);

    return real_file.string().find(real_root.string()) == 0;
}

