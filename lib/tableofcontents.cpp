#include "tableofcontents.h"
#include "directoryentry.h"
#include "regularfileentry.h"
#include "linkentry.h"
#include "packager.h"
#include "decompressorfactory.h"
#include "internal_definitions.h"
#include "logger_internal.h"

#include <boost/filesystem.hpp>

#include <chrono>

using namespace parts;

const std::string TableOfContents::DEFAULT_OWNER = "__PARTS_DEFAULT_OWNER__";
const std::string TableOfContents::DEFAULT_GROUP = "__PARTS_DEFAULT_GROUP__";

//==========================================================================================================================================
TableOfContents::TableOfContents(const boost::filesystem::path& source, const PartsCompressionParameters& parameters) noexcept:
    m_parameters(parameters),
    m_maxSize(0),
    m_maxOwnerWidth(0)
{
    LOG_TRACE("Source: {}", source.string());
    m_owners.push_back(DEFAULT_OWNER);
    m_groups.push_back(DEFAULT_GROUP);

    if (source.string().empty())
    {
        LOG_INFO ("Empty source directory provided, skip scanning directory.");
        return;
    }

    if (!boost::filesystem::exists(source)) {
        LOG_ERROR("Source doesn't exist: {}", source.string());
        return;
    }

    try {
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
    } catch(const std::exception& e) {
        LOG_ERROR("Error during creating TOC for file: {}, reason: {}", source.string(), e.what());
        // signs that TOC is invalid
        m_files.clear();
    }
}

//==========================================================================================================================================
TableOfContents::TableOfContents(ContentReadBackend& backend, size_t toc_size, const PartsCompressionParameters& parameters) noexcept:
    m_parameters(parameters),
    m_maxSize(0),
    m_maxOwnerWidth(0)
{
    try {
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
                m_maxSize = std::max(m_maxSize, std::dynamic_pointer_cast<RegularFileEntry>(entry)->uncompressedSize());
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

            m_maxOwnerWidth = std::max(m_maxOwnerWidth, entry->owner().size() + entry->group().size() + 1);

            LOG_DEBUG("TOC entry: {}", entry->toString());
            m_files[entry->file()] = entry;
        }
    } catch(const std::exception& e) {
        LOG_ERROR("Cannot create toc from backend: {}, reason: {}", backend.source(), e.what());
        // sign that TOC is invalid
        m_files.clear();
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
std::shared_ptr<BaseEntry> TableOfContents::find(const boost::filesystem::path& file) const
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

    std::shared_ptr<BaseEntry> entry;
    // must check this first, because depending of the target of the link is_regular_file and is_directory are also true...
    if (boost::filesystem::is_symlink(file)) {
        entry.reset(new LinkEntry(root, filename, m_owners, m_groups, m_parameters.m_saveOwners));
    } else if (boost::filesystem::is_directory(file)) {
        entry.reset(new DirectoryEntry(root, filename, m_owners, m_groups, m_parameters.m_saveOwners));
    } else if (boost::filesystem::is_regular_file(file)) {
        entry.reset(new RegularFileEntry(root, filename, m_owners, m_groups, m_parameters.m_saveOwners, m_parameters));
    } else {
        LOG_WARNING("Unknown file type: {}", file.string());
        return;
    }
    LOG_DEBUG("Adding TOC entry: {}", entry->toString());

    m_files[entry->file()] = entry;
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
TableOfContents::TableOfContents(const PartsCompressionParameters& params) noexcept : m_parameters(params)
{
    // Unit test only constructor, It does nothing
}
