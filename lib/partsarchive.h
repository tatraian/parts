#ifndef PARTSARCHIVE_H
#define PARTSARCHIVE_H

#include <boost/filesystem/path.hpp>
#include "parts_definitions.h"
#include "header.h"
#include "tableofcontents.h"
#include "contentreadbackend.h"

namespace parts
{

class PartsArchive
{
public:
    /**
     * @brief PartsArchive constructor to compress content
     * @param root The root of compression
     * @param parameters The compression parameters
     */
    PartsArchive(const boost::filesystem::path& root, const PartsCompressionParameters& parameters);

    /**
     * @brief PartsArchive constructor for extracting archive
     * @param backend Backend should now the destination
     */
    PartsArchive(std::unique_ptr<ContentReadBackend>&& backend);


    const TableOfContents& toc() const
    {return m_toc; }

    void createArchive(const boost::filesystem::path& archive);

    void extractArchive(const boost::filesystem::path& dest) const;
    void updateArchive(const boost::filesystem::path& original_root,
                       const boost::filesystem::path& dest);

protected:
    // destination for write mode
    boost::filesystem::path m_root;
    // reader interface for extract/update mode
    std::unique_ptr<ContentReadBackend> m_contentReader;

    Header m_header;
    TableOfContents m_toc;
};

}
#endif // PARTSARCHIVE_H
