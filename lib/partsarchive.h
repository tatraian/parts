#ifndef PARTSARCHIVE_H
#define PARTSARCHIVE_H

#include <boost/filesystem/path.hpp>
#include "parts_definitions.h"
#include "header.h"
#include "tableofcontents.h"
#include "contentreadbackend.h"
#include "partsjobinterface.h"

namespace parts
{

class PartsArchive
{
public:
    /**
     * @brief PartsArchive constructor to compress content
     * @param target The source of compression
     * @param parameters The compression parameters
     */
    PartsArchive(const boost::filesystem::path& source, const PartsCompressionParameters& parameters);

    /**
     * @brief PartsArchive constructor for extracting archive
     * @param backend Backend should now the destination
     */
    PartsArchive(std::unique_ptr<ContentReadBackend>&& backend, const PartsCompressionParameters& parameters);

    const TableOfContents& toc() const
    { return m_toc; }

    /** Don't set it to const, the whole idea is to be able to set it from the outside world */
    PartsCompressionParameters & compressionParameters()
    { return m_compressionParameters; }

    void listArchive(std::ostream& output) const;

    void createArchive(const boost::filesystem::path& archive);

    void extractArchive(const boost::filesystem::path& dest);
    /** checkExisting is to continue an aborted/failed update */
    void updateArchive(const boost::filesystem::path& original_source,
                       const boost::filesystem::path& dest,
                       bool checkExisting = false);

    // Job API!
    // Take care auto job = updateJob(...);
    // while(*job)
    //     job->doNext()
    // without '*' you check the unique_ptr...
    std::unique_ptr<PartsJobInterface> updateJob(const boost::filesystem::path& original_source,
                                                 const boost::filesystem::path& dest);

    uint64_t readBytes() const
    { if (m_contentReader) return m_contentReader->readBytes(); else return 0; }
    uint64_t sentRequests() const
    { if (m_contentReader) return m_contentReader->sentRequests(); else return 0; }

    bool extractToMc(const boost::filesystem::path& file_path, const boost::filesystem::path& dest_file);

    std::string source() const
    { if (m_contentReader) return m_contentReader->source(); else return ""; }

protected:
    // destination for write mode
    boost::filesystem::path m_root;
    // reader interface for extract/update mode
    std::unique_ptr<ContentReadBackend> m_contentReader;

    Header m_header;

    // Compression parameters (in case of compression)
    PartsCompressionParameters m_compressionParameters;

    TableOfContents m_toc;
};

}
#endif // PARTSARCHIVE_H
