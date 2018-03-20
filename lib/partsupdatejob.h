#ifndef PARTSUPDATEJOB_H
#define PARTSUPDATEJOB_H

#include "partsjobinterface.h"
#include "parts_definitions.h"
#include "tableofcontents.h"

namespace parts
{

class PartsUpdateJob : public PartsJobInterface
{
public:
    PartsUpdateJob(HashType hash_type,
                   CompressionType compression_type,
                   TableOfContents& new_toc,
                   const boost::filesystem::path& orig_source,
                   const boost::filesystem::path& dest,
                   ContentReadBackend& content_reader);

    ~PartsUpdateJob() = default;

    operator bool() const override
    { return m_actualElement != m_toc.end(); }

    const BaseEntry* nextEntry() const override
    { return m_actualElement->second.get(); }

    void doNext(bool checkExisting) override;

protected:
    CompressionType m_compressionType;

    std::string m_oldRootName;
    std::string m_rootname;

    TableOfContents m_oldToc;
    TableOfContents& m_toc;

    TableOfContents::iterator m_actualElement;
    boost::filesystem::path m_oldRootDir;
    boost::filesystem::path m_dest;
    ContentReadBackend& m_contentReader;
};

}

#endif // PARTSUPDATEJOB_H
