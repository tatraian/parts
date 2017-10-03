#ifndef TABLEOFCONTENTS_H
#define TABLEOFCONTENTS_H

#include "baseentry.h"

#include <map>
#include <memory>
#include <boost/filesystem/path.hpp>

namespace parts
{

class TableOfContents
{
public:
    TableOfContents(const boost::filesystem::path& root);

    std::vector<uint8_t> getRaw() const;

protected:
    std::map<boost::filesystem::path, std::shared_ptr<BaseEntry> > m_files;
};

}

#endif // TABLEOFCONTENTS_H
