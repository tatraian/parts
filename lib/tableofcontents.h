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

#include <sys/stat.h>

namespace parts
{

class TableOfContents
{
public:
    typedef std::map<boost::filesystem::path, std::shared_ptr<BaseEntry>>::iterator iterator;
    typedef std::map<boost::filesystem::path, std::shared_ptr<BaseEntry>>::const_iterator const_iterator;

    TableOfContents(const boost::filesystem::path& source, const PartsCompressionParameters& parameters);
    TableOfContents(ContentReadBackend& backend, size_t toc_size, const PartsCompressionParameters& parameters);

    std::vector<uint8_t> getRaw() const;

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

    std::shared_ptr<BaseEntry> find(const boost::filesystem::path& file);

    static const std::string DEFAULT_OWNER;
    static const std::string DEFAULT_GROUP;


protected:
    void add(const boost::filesystem::path& root, const boost::filesystem::path& file);
    uint16_t getOwnerId(const struct stat* file_stat);
    uint16_t getGroupId(const struct stat* file_stat);
    uint16_t getPermissions(const struct stat* file_stat);

    uint16_t findOrInsert(const std::string& name, std::vector<std::string>& table);

    static bool fileInsideRoot(const boost::filesystem::path& root, const boost::filesystem::path& file);

    void packNames(std::vector<uint8_t>& buffer, const std::vector<std::string>& names) const;
    void unpackNames(InputBuffer& buffer, std::vector<std::string>& names);

    // This constructor is just for unit testing...
    TableOfContents(const PartsCompressionParameters& params);

protected:
    std::map<boost::filesystem::path, std::shared_ptr<BaseEntry> > m_files;
    std::vector<std::string> m_owners;
    std::vector<std::string> m_groups;

    PartsCompressionParameters m_parameters;
};

}

#endif // TABLEOFCONTENTS_H
