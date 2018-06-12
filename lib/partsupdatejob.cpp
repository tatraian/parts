#include "partsupdatejob.h"

#include "decompressorfactory.h"

using namespace parts;

//==========================================================================================================================================
PartsUpdateJob::PartsUpdateJob(HashType hash_type,
                               CompressionType compression_type,
                               TableOfContents& new_toc,
                               const boost::filesystem::path& orig_source,
                               const boost::filesystem::path& dest, ContentReadBackend& content_reader,
                               bool cont) :
    m_compressionType(compression_type),
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
    boost::filesystem::path p;
    std::string path_string = m_actualElement->first.string();
    p = path_string.replace(path_string.find(m_rootname), m_rootname.size(), m_oldRootName);

    auto old_entry = m_oldToc.find(p);

    auto decompressor = DecompressorFactory::createDecompressor(m_compressionType);
    m_actualElement->second->updateEntry(old_entry.get(),
                                         m_oldRootDir,
                                         m_dest,
                                         *decompressor,
                                         m_contentReader,
                                         m_continue);
    ++m_actualElement;
}
