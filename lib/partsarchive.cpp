#include "partsarchive.h"
#include "filewritebackend.h"
#include "compressorfactory.h"
#include "decompressorfactory.h"
#include "logger_internal.h"
#include <chrono>

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
    uint64_t head_and_compressed_toc_size = m_contentReader->position();
    m_toc.shiftOffsets(head_and_compressed_toc_size);
}

//==========================================================================================================================================
void PartsArchive::createArchive(const boost::filesystem::path& archive)
{
    auto p = boost::filesystem::unique_path();
    p = "/tmp" / p;

    LOG_DEBUG("Uniq temprary file: {}", p.string());
    FileWriteBackend temp(p);

    for(auto& entry : m_toc) {
        auto compressor = CompressorFactory::createCompressor(m_compressionParameters.m_fileCompression, m_compressionParameters);
        entry.second->compressEntry(m_root, *compressor, temp);
    }

    LOG_DEBUG("Files compressed sum: {}", temp.getPosition());
    FileWriteBackend file(archive.string());
    std::vector<uint8_t> uncompressed_toc = m_toc.getRaw();
    LOG_DEBUG("Uncompressed toc size: {}", uncompressed_toc.size());

    auto toc_compressior = CompressorFactory::createCompressor(m_compressionParameters.m_tocCompression, m_compressionParameters);
    std::vector<uint8_t> compressed_toc;
    LOG_DEBUG("Compressing TOC");
    toc_compressior->compressBuffer(uncompressed_toc, compressed_toc);
    m_header.setTocSize(compressed_toc.size());
    LOG_DEBUG("Compressed TOC size: {}", compressed_toc.size());

    file.append(m_header.getRaw());
    file.append(compressed_toc);
    file.concatenate(std::move(temp));
}

//==========================================================================================================================================
void PartsArchive::extractArchive(const boost::filesystem::path& dest) const
{
    for (auto& entry : m_toc) {
        auto decompressor = DecompressorFactory::createDecompressor(m_header.getFileCompressionType());
        entry.second->extractEntry(dest, *decompressor, *m_contentReader.get());
    }
}

//==========================================================================================================================================
void PartsArchive::updateArchive(const boost::filesystem::path& original_source, const boost::filesystem::path& dest)
{
    if (m_toc.size() == 0)
        return;

    std::string rootname = m_toc.begin()->first.string();

    PartsCompressionParameters params;
    params.m_hashType = m_header.getHashType();
    TableOfContents old_toc(original_source, params);

    std::string old_rootname;
    if (old_toc.size() != 0)
        old_rootname = old_toc.begin()->first.string();

    for (auto& entry : m_toc) {
        boost::filesystem::path p;
        std::string path_string = entry.first.string();
        p = path_string.replace(path_string.find(rootname), rootname.size(), old_rootname);

        auto old_entry = old_toc.find(p);

        auto decompressor = DecompressorFactory::createDecompressor(m_header.getFileCompressionType());
        entry.second->updateEntry(old_entry.get(), original_source.parent_path(), dest, *decompressor, *m_contentReader.get());
    }
}
