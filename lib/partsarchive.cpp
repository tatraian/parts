#include "partsarchive.h"
#include "filewritebackend.h"
#include "lzmacompressor.h"
#include "lzmadecompressor.h"
#include "logger.h"

#include <boost/filesystem.hpp>

using namespace parts;


//==========================================================================================================================================
PartsArchive::PartsArchive(const boost::filesystem::path& source, const PartsCompressionParameters& parameters) :
    m_root(source.parent_path()),
    m_header(parameters),
    m_toc(source, parameters),
    m_compressionParameters(parameters)
{
}

//==========================================================================================================================================
PartsArchive::PartsArchive(std::unique_ptr<ContentReadBackend>&& backend) :
    m_contentReader(std::move(backend)),
    m_header(*m_contentReader.get()),
    m_toc(*m_contentReader.get(), m_header.getTocSize(), PartsCompressionParameters(m_header.getHashType(),
                                                                                    m_header.getTocCompressionType(),
                                                                                    m_header.getFileCompressionType()))
{
}

//==========================================================================================================================================
void PartsArchive::createArchive(const boost::filesystem::path& archive)
{
    auto p = boost::filesystem::unique_path();
    p = "/tmp" / p;

    LOG_DEBUG("Uniq temprary file: {}", p.string());
    FileWriteBackend temp(p);

    LzmaCompressorParameters lzma_pars;
    for(auto& entry : m_toc) {
        LOG_TRACE("Compressing entry: {}", entry.first.string());
        LzmaCompressor compressior(m_compressionParameters.m_lzmaParameters);
        entry.second->compressEntry(m_root, compressior, temp);
    }

    FileWriteBackend file(archive.string());
    std::vector<uint8_t> uncompressed_toc = m_toc.getRaw();

    LzmaCompressor toc_compressior(m_compressionParameters.m_lzmaParameters);
    std::vector<uint8_t> compressed_toc;
    LOG_DEBUG("Compressing TOC");
    toc_compressior.compressBuffer(uncompressed_toc, compressed_toc);
    m_header.setTocSize(compressed_toc.size());

    LOG_DEBUG("Concatenating files");
    file.append(m_header.getRaw());
    file.append(compressed_toc);
    file.concatenate(std::move(temp));
}

//==========================================================================================================================================
void PartsArchive::extractArchive(const boost::filesystem::path& dest) const
{
    for (auto& entry : m_toc) {
        LzmaDecompressor decompressor;
        entry.second->extractEntry(dest, decompressor, *m_contentReader.get());
    }
}
