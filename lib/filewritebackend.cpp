#include "filewritebackend.h"

#include "parts_definitions.h"

#include <ios>
#include <iostream>

#include <boost/endian/conversion.hpp>
#include <boost/filesystem.hpp>

using namespace parts;

namespace
{
const size_t MB_4 = 1024 * 1024 * 4;

//==========================================================================================================================================
template<class File, class Value>
void append(File& file, Value value) {
    if (!file.is_open())
        throw PartsException("Internal Error: file has been closed!");


    boost::endian::native_to_big_inplace(value);
    file.write(reinterpret_cast<char*>(&value), sizeof(value));
}

}

//==========================================================================================================================================
FileWriteBackend::FileWriteBackend(const boost::filesystem::path& file) :
    m_filename(file)
{
    m_file.open(file.string(),std::ios::out | std::ios::ate | std::ios::binary);
    if (!m_file.is_open())
        throw PartsException("Cannot open file for writing: " + file.string());
}

//==========================================================================================================================================
FileWriteBackend::~FileWriteBackend()
{
    m_file.flush();
    m_file.close();
}

//==========================================================================================================================================
void FileWriteBackend::append(uint8_t value)
{
    ::append(m_file, value);
}

//==========================================================================================================================================
void FileWriteBackend::append(uint16_t value)
{
    ::append(m_file, value);
}

//==========================================================================================================================================
void FileWriteBackend::append(uint32_t value)
{
    ::append(m_file, value);
}

//==========================================================================================================================================
void FileWriteBackend::append(uint64_t value)
{
    ::append(m_file, value);
}

//==========================================================================================================================================
void FileWriteBackend::append(const std::vector<uint8_t>& value)
{
    checkFile();

    m_file.write(reinterpret_cast<const char*>(value.data()), value.size());
}

//==========================================================================================================================================
uint64_t FileWriteBackend::getPosition()
{
    checkFile();

    return m_file.tellp();
}

//==========================================================================================================================================
void FileWriteBackend::concatenate(ContentWriteBackend&& tail)
{
    checkFile();

    if (dynamic_cast<FileWriteBackend*>(&tail) == 0)
        throw PartsException("Internal Error: FileWriteBackend can concatenate only the same type of object!");

    auto&& file_tail = dynamic_cast<FileWriteBackend&&>(tail);
    file_tail.resetToRead();
    std::vector<uint8_t> tmp;
    tmp.reserve(MB_4);
    for(;;) {
        file_tail.read(tmp);
        if (tmp.empty())
            break;
        append(tmp);
    }

    // This will drop this object in invalid state
    file_tail.m_file.close();
    boost::filesystem::remove(file_tail.m_filename);
}

//==========================================================================================================================================
void FileWriteBackend::resetToRead()
{
    checkFile();

    m_file.flush();
    m_file.close();

    m_file.open(m_filename.string(),std::ios::in | std::ios::binary);
}

//==========================================================================================================================================
void FileWriteBackend::read(std::vector<uint8_t>& buffer)
{
    checkFile();

    buffer.resize(MB_4);
    m_file.read(reinterpret_cast<char*>(buffer.data()), MB_4);
    buffer.resize(m_file.gcount());
}



//==========================================================================================================================================
void FileWriteBackend::checkFile() const
{
    if (!m_file.is_open())
        throw PartsException("Internal Error: file has been closed!");
}
