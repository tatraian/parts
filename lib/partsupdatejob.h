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
                   TableOfContents& new_toc,
                   const std::filesystem::path& orig_source,
                   const std::filesystem::path& dest,
                   ContentReadBackend& content_reader,
                   bool cont) noexcept;

    ~PartsUpdateJob() noexcept override = default;

    operator bool() const override
    { return m_actualElement != m_toc.end(); }

    const BaseEntry* nextEntry() const override
    { return m_actualElement->second.get(); }

    void doNext() override;

    void skip() override;

protected:
    std::string m_oldRootName;
    std::string m_rootname;

    TableOfContents m_oldToc;
    TableOfContents& m_toc;

    TableOfContents::iterator m_actualElement;
    std::filesystem::path m_oldRootDir;
    std::filesystem::path m_dest;
    ContentReadBackend& m_contentReader;
    bool m_continue;
};

}

#endif // PARTSUPDATEJOB_H
