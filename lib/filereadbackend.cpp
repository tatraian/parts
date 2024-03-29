#include "filereadbackend.h"
#include "parts_definitions.h"
#include "logger_internal.h"

#include <filesystem>

using namespace parts;

//==========================================================================================================================================
FileReadBackend::FileReadBackend(const std::filesystem::path& filename) :
    m_path(filename),
    m_file(filename.string(), std::ios::binary)
{
    if (!m_file) {
        LOG_ERROR("Cannot open file for reading: {}", filename.string());
        return;
    }

    // This is removed from initializer list to avoid boost exception in case of non existing files.
    m_filesize = std::filesystem::file_size(m_path);
    m_valid = true;
}

//==========================================================================================================================================
void FileReadBackend::read(std::vector<uint8_t>& data)
{
    read(data.data(), data.size());
}

//==========================================================================================================================================
void FileReadBackend::read(InputBuffer& data, size_t size)
{
    size_t old_size = data.size();
    data.append(size);
    read(&(*(data.begin() + old_size)), size);
}

//==========================================================================================================================================
void FileReadBackend::read(uint8_t* data, size_t size)
{
    m_file.read(reinterpret_cast<char*>(data), size);
    if (static_cast<size_t>(m_file.gcount()) != size)
        throw PartsException("Not enough data to read in file: " + m_path.string());
    m_readBytes += size;
}

//==========================================================================================================================================
void FileReadBackend::seek(const uint64_t& position)
{
    if (position >= m_filesize)
        throw PartsException("Seeking file: " + m_path.string() + " failed!");
    m_file.seekg(position);
    ++m_sentRequests;
}

//==========================================================================================================================================
uint64_t FileReadBackend::position()
{
    return m_file.tellg();
}
