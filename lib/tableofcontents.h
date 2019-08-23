#ifndef TABLEOFCONTENTS_H
#define TABLEOFCONTENTS_H

#include "baseentry.h"
#include "parts_definitions.h"
#include "contentreadbackend.h"

#include <map>
#include <memory>
#include <vector>
#include <string>

#include <boost/filesystem/path.hpp>

namespace parts
{

class TableOfContents
{
public:
    typedef std::map<boost::filesystem::path, std::shared_ptr<BaseEntry>>::iterator iterator;
    typedef std::map<boost::filesystem::path, std::shared_ptr<BaseEntry>>::const_iterator const_iterator;

    /**
     * The only reasons for these default variants is to support 3rd party TOC handling, so that
     * 3rd party implementations can directly inherit from this
     */
    TableOfContents() = default;
    virtual ~TableOfContents() = default;

    TableOfContents(const TableOfContents&) = default;
    TableOfContents& operator=(const TableOfContents&) = default;

    TableOfContents(TableOfContents&&) = default;
    TableOfContents& operator=(TableOfContents&&) = default;

    TableOfContents(const boost::filesystem::path& source, const PartsCompressionParameters& parameters) noexcept;
    TableOfContents(ContentReadBackend& backend, size_t toc_size, const PartsCompressionParameters& parameters) noexcept;

    std::vector<uint8_t> getRaw() const;

    void shiftOffsets(uint64_t& data_start);

    iterator begin()
    { return m_files.begin(); }
    const_iterator begin() const
    { return m_files.begin(); }

    iterator end()
    { return m_files.end(); }
    const_iterator end() const
    { return m_files.end(); }

    size_t size() const
    { return m_files.size(); }

    std::shared_ptr<BaseEntry> find(const boost::filesystem::path& file) const;

    uint64_t maxSize() const
    { return m_maxSize; }

    size_t maxOwnerGroupWidth() const
    { return m_maxOwnerWidth; }

    operator bool() const
    { return !m_files.empty(); }

    static const std::string DEFAULT_OWNER;
    static const std::string DEFAULT_GROUP;


protected:
    virtual void add(const boost::filesystem::path& root, const boost::filesystem::path& file);

    void packNames(std::vector<uint8_t>& buffer, const std::vector<std::string>& names) const;
    void unpackNames(InputBuffer& buffer, std::vector<std::string>& names);

    // This constructor is just for unit testing...
    TableOfContents(const PartsCompressionParameters& params) noexcept;

protected:
    std::map<boost::filesystem::path, std::shared_ptr<BaseEntry> > m_files;
    std::vector<std::string> m_owners;
    std::vector<std::string> m_groups;

    PartsCompressionParameters m_parameters;

    uint64_t m_maxSize;
    size_t m_maxOwnerWidth;
};

}

#endif // TABLEOFCONTENTS_H
