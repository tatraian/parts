#ifndef PARTSARCHIVE_H
#define PARTSARCHIVE_H

#include <boost/filesystem/path.hpp>
#include "parts_definitions.h"
#include "header.h"
#include "tableofcontents.h"

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
     * @param archive
     */
    //PartsArchive(const boost::filesystem::path& archive);


    const TableOfContents& toc() const
    {return m_toc; }

    void createArchive(const boost::filesystem::path& archive);
/* This is not final, because they should be get the "Reader interface too"
    void extractArchive(const boost::filesystem::path& dest, const boost::filesystem::path& archive) const;
    void updateArchive(const boost::filesystem::path& original_root,
                       const boost::filesystem::path& dest,
                       const boost::filesystem::path& archive);
*/
protected:
    const boost::filesystem::path& m_root;
    TableOfContents m_toc;
    Header m_header;
};

}
#endif // PARTSARCHIVE_H
