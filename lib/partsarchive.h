#ifndef PARTSARCHIVE_H
#define PARTSARCHIVE_H

#include <filesystem>
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
    PartsArchive(const std::filesystem::path& source, const PartsCompressionParameters& parameters) noexcept;

    /**
     * @brief PartsArchive constructor for extracting archive
     * @param backend Backend should now the destination
     */
    PartsArchive(std::unique_ptr<ContentReadBackend>&& backend) noexcept;

    PartsArchive(const PartsArchive&) = delete;
    PartsArchive& operator=(const PartsArchive&) = delete;

    PartsArchive(PartsArchive&&) = default;
    PartsArchive& operator=(PartsArchive&&) = default;

    const TableOfContents& toc() const
    { return *m_toc; }

    void listArchive(std::ostream& output) const;

    void createArchive(const std::filesystem::path& archive);

    void extractArchive(const std::filesystem::path& dest, bool cont);
    void updateArchive(const std::filesystem::path& original_source,
                       const std::filesystem::path& dest,
                       bool cont);

    // Job API!
    // Take care auto job = updateJob(...);
    // while(*job)
    //     job->doNext()
    // without '*' you check the unique_ptr...
    std::unique_ptr<PartsJobInterface> updateJob(const std::filesystem::path& original_source,
                                                 const std::filesystem::path& dest,
                                                 bool cont);

    uint64_t readBytes() const
    { if (m_contentReader) return m_contentReader->readBytes(); else return 0; }
    uint64_t sentRequests() const
    { if (m_contentReader) return m_contentReader->sentRequests(); else return 0; }

    bool extractToMc(const std::filesystem::path& file_path, const std::filesystem::path& dest_file);

    operator bool() const
    { return *m_toc; }

protected:
    // destination for write mode
    std::filesystem::path m_root;
    // reader interface for extract/update mode
    std::unique_ptr<ContentReadBackend> m_contentReader;

    std::unique_ptr<Header> m_header;

    // Compression parameters (in case of compression)
    PartsCompressionParameters m_compressionParameters;

    std::unique_ptr<TableOfContents> m_toc;
};

}
#endif // PARTSARCHIVE_H
