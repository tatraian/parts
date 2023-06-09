#include "partsupdatejob.h"

using namespace parts;

//==========================================================================================================================================
PartsUpdateJob::PartsUpdateJob(HashType hash_type,
                               TableOfContents& new_toc,
                               const std::filesystem::path& orig_source,
                               const std::filesystem::path& dest,
                               ContentReadBackend& content_reader,
                               bool cont) noexcept :
    m_oldToc(orig_source, PartsCompressionParameters(hash_type)),
    m_toc(new_toc),
    m_actualElement(m_toc.begin()),
    m_oldRootDir(orig_source.parent_path()),
    m_dest(dest),
    m_contentReader(content_reader),
    m_continue(cont)
{
    if (m_toc.size() == 0)
        return;

    m_rootname = m_toc.begin()->first.string();

    if (m_oldToc.size() != 0)
        m_oldRootName = m_oldToc.begin()->first.string();
}

//==========================================================================================================================================
void PartsUpdateJob::doNext()
{
    if (m_actualElement == m_toc.end())
    {
        throw PartsException ("Job error, already finished.");
    }

    std::filesystem::path p;
    std::string path_string = m_actualElement->first.string();
    p = path_string.replace(path_string.find(m_rootname), m_rootname.size(), m_oldRootName);

    auto old_entry = m_oldToc.find(p);

    m_actualElement->second->updateEntry(old_entry.get(),
                                         m_oldRootDir,
                                         m_dest,
                                         m_contentReader,
                                         m_continue);
    ++m_actualElement;
}

//==========================================================================================================================================
void PartsUpdateJob::skip()
{
    if (m_actualElement == m_toc.end())
    {
        throw PartsException ("Job error, already finished.");
    }

    ++m_actualElement;
}
