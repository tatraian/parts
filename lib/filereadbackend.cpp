#include "filereadbackend.h"
#include "parts_definitions.h"

#include <boost/endian/conversion.hpp>
#include <boost/filesystem.hpp>

using namespace parts;

namespace {

//==========================================================================================================================================
template<class Value>
void read_internal(Value& value, std::ifstream& file) {
    file.read(reinterpret_cast<char*>(&value), sizeof(Value));
    if (file.gcount() != sizeof(Value))
        throw PartsException("Not enough data in file");
    boost::endian::big_to_native_inplace(value);
}

}

//==========================================================================================================================================
FileReadBackend::FileReadBackend(const boost::filesystem::path& filename) :
    m_path(filename),
    m_file(filename.string(), std::ios::binary)
{
    if (!m_file)
        throw PartsException("Cannot open file for reading: " + filename.string());

    // This is removed from initializer list to avoid boost exception in case of non existing files.
    m_filesize = boost::filesystem::file_size(m_path);
}

//==========================================================================================================================================
void FileReadBackend::read(uint8_t& data)
{
    read_internal(data, m_file);
}

//==========================================================================================================================================
void FileReadBackend::read(uint16_t& data)
{
    read_internal(data, m_file);
}

//==========================================================================================================================================
void FileReadBackend::read(uint32_t& data)
{
    read_internal(data, m_file);
}

//==========================================================================================================================================
void FileReadBackend::read(uint64_t& data)
{
    read_internal(data, m_file);
}

//==========================================================================================================================================
void FileReadBackend::read(std::vector<uint8_t>& data)
{
    m_file.read(reinterpret_cast<char*>(data.data()), data.size());
    if (m_file.gcount() != data.size())
        throw PartsException("Not enough data to read in file: " + m_path.string());
}

//==========================================================================================================================================
void FileReadBackend::read(uint8_t* data, size_t size)
{
    m_file.read(reinterpret_cast<char*>(data), size);
    if (m_file.gcount() != size)
        throw PartsException("Not enough data to read in file: " + m_path.string());
}

//==========================================================================================================================================
void FileReadBackend::seek(const uint64_t& position)
{
    if (position >= m_filesize)
        throw PartsException("Seeking file: " + m_path.string() + " failed!");
    m_file.seekg(position);
}

//==========================================================================================================================================
uint64_t FileReadBackend::position()
{
    return m_file.tellg();
}
