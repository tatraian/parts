#include "partsarchive.h"
#include "filewritebackend.h"
#include "lzmacompressor.h"

#include <boost/filesystem.hpp>

using namespace parts;


//==========================================================================================================================================
PartsArchive::PartsArchive(const boost::filesystem::path& root, const PartsCompressionParameters& parameters) :
    m_root(root),
    m_header(parameters),
    m_toc(root, parameters)
{
}

//==========================================================================================================================================
PartsArchive::PartsArchive(std::unique_ptr<ContentReadBackend>&& backend) :
    m_contentReader(std::move(backend)),
    m_header(*backend.get()),
    m_toc(*backend.get(), m_header.getTocSize(), PartsCompressionParameters(m_header.getHashType(),
                                                                            m_header.getTocCompressionType(),
                                                                            m_header.getFileCompressionType()))
{
}

//==========================================================================================================================================
void PartsArchive::createArchive(const boost::filesystem::path& archive)
{
    auto p = boost::filesystem::unique_path();
    FileWriteBackend temp(p);
    // TODO: log for temp path
    LzmaCompressorParameters lzma_pars;
    for(auto& entry : m_toc) {
        // TODO log about processing file
        LzmaCompressor compressior(lzma_pars);
        entry.second->compressEntry(m_root, compressior, temp);
    }

    FileWriteBackend file(archive.string());
    std::vector<uint8_t> uncompressed_toc = m_toc.getRaw();

    LzmaCompressor toc_compressior(lzma_pars);
    std::vector<uint8_t> compressed_toc;
    toc_compressior.compressBuffer(uncompressed_toc, compressed_toc);
    m_header.setTocSize(compressed_toc.size());

    file.append(m_header.getRaw());
    file.append(compressed_toc);
    file.concatenate(std::move(temp));
}
