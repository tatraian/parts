#ifndef TABLEOFCONTENTS_H
#define TABLEOFCONTENTS_H

#include "baseentry.h"
#include "parts_definitions.h"

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
    TableOfContents(const boost::filesystem::path& root, const PartsCompressionParameters& parameters);

    std::vector<uint8_t> getRaw() const;

protected:
    uint16_t getOwnerId(const stat* file_stat);
    uint16_t getGroupId(const stat* file_stat);
    uint16_t getPermissions(const stat* file_stat);

protected:
    std::map<boost::filesystem::path, std::shared_ptr<BaseEntry> > m_files;
    std::vector<std::string> m_owners;
    std::vector<std::string> m_groups;

    const PartsCompressionParameters& m_parameters;
};

}

#endif // TABLEOFCONTENTS_H
