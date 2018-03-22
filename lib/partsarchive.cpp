#include "partsarchive.h"
#include "filewritebackend.h"
#include "compressorfactory.h"
#include "decompressorfactory.h"
#include "logger_internal.h"
#include "partsupdatejob.h"
#include <chrono>
#include <ctime>

#include <boost/filesystem.hpp>

using namespace parts;


//==========================================================================================================================================
PartsArchive::PartsArchive(const boost::filesystem::path& source, const PartsCompressionParameters& parameters) :
    m_root(source.parent_path()),
    m_header(parameters),
    m_compressionParameters(parameters),
    m_toc(source, parameters)
{
}

//==========================================================================================================================================
PartsArchive::PartsArchive(std::unique_ptr<ContentReadBackend>&& backend, const PartsCompressionParameters& parameters) :
    m_contentReader(std::move(backend)),
    m_header(*m_contentReader.get()),
    m_compressionParameters(m_header.getHashType(), m_header.getTocCompressionType(), m_header.getFileCompressionType(), parameters.m_saveOwners, parameters.m_compareHash),
    m_toc(*m_contentReader.get(), m_header.getTocSize(), m_header.getDecompressedTocSize(), m_compressionParameters)
{
    uint64_t head_and_compressed_toc_size = m_contentReader->position();
    m_toc.shiftOffsets(head_and_compressed_toc_size);
}

//==========================================================================================================================================
void PartsArchive::listArchive(std::ostream& output) const
{
    time_t now = time(nullptr);
    std::tm* t = std::localtime(&now);
    for(auto name_entry : m_toc) {
        BaseEntry* entry = name_entry.second.get();
        output << entry->listEntry(m_toc.maxOwnerGroupWidth(), std::to_string(m_toc.maxSize()).size(), t) << std::endl;
    }
}

//==========================================================================================================================================
void PartsArchive::createArchive(const boost::filesystem::path& archive)
{
    auto p = boost::filesystem::unique_path();
    p = "/tmp" / p;

    LOG_DEBUG("Uniq temprary file: {}", p.string());
    FileWriteBackend temp(p);

    for(auto& entry : m_toc) {
        entry.second->compressEntry(m_root, temp);
    }

    LOG_DEBUG("Files compressed sum: {}", temp.getPosition());
    FileWriteBackend file(archive.string());
    std::vector<uint8_t> uncompressed_toc = m_toc.getRaw();
    LOG_DEBUG("Uncompressed toc size: {}", uncompressed_toc.size());

    auto toc_compressior = CompressorFactory::createCompressor(m_compressionParameters.m_tocCompression, m_compressionParameters);
    std::vector<uint8_t> compressed_toc;
    LOG_DEBUG("Compressing TOC");
    toc_compressior->compressBuffer(uncompressed_toc, compressed_toc);
    m_header.setDecompressedTocSize(uncompressed_toc.size());
    m_header.setTocSize(compressed_toc.size());
    LOG_DEBUG("Compressed TOC size: {}", compressed_toc.size());

    file.append(m_header.getRaw());
    file.append(compressed_toc);
    file.concatenate(std::move(temp));
}

//==========================================================================================================================================
void PartsArchive::extractArchive(const boost::filesystem::path& dest)
{
    for (auto& entry : m_toc) {
        entry.second->extractEntry(dest, *m_contentReader.get());
    }
}

//==========================================================================================================================================
void PartsArchive::updateArchive(const boost::filesystem::path& original_source, const boost::filesystem::path& dest, bool checkExisting)
{
    PartsUpdateJob job(m_header.getHashType(), m_header.getFileCompressionType(), m_toc, original_source, dest, *m_contentReader.get());

    while(job) {
        job.doNext(checkExisting);
    }
}

//==========================================================================================================================================
std::unique_ptr<PartsJobInterface> PartsArchive::updateJob(const boost::filesystem::path& original_source, const boost::filesystem::path& dest)
{
    return std::make_unique<PartsUpdateJob>(m_header.getHashType(),
                                            m_header.getFileCompressionType(),
                                            m_toc,
                                            original_source,
                                            dest,
                                            *m_contentReader.get());

}

//==========================================================================================================================================
bool PartsArchive::extractToMc(const boost::filesystem::path& file_path, const boost::filesystem::path& dest_file)
{
    auto entry = m_toc.find(file_path);
    if (!entry) {
        return false;
    }

    return entry->extractToMc(dest_file, *m_contentReader.get());
}
