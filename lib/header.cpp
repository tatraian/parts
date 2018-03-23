#include "header.h"

#include "packager.h"

using namespace parts;

namespace {
char magic[] = {'p','a','r','t','s','!'};
}

//==========================================================================================================================================
Header::Header(const PartsCompressionParameters& parameters) :
    m_magic(magic),
    m_version(1),
    m_tocCompressionType(parameters.m_tocCompression),
    m_fileCompressionType(parameters.m_fileCompression),
    m_hashType(parameters.m_hashType),
    m_tocSize(0),
    m_decompressedTocSize(0)
{
}

bool Header::checkMagic(const std::vector<uint8_t> & header, size_t offset) const
{
    if (offset + 6 > header.size())
        throw PartsException ("Too short header record");

    for(size_t tmp = 0; tmp != 6; ++tmp)
        if (magic[tmp] != header[offset + tmp])
            return false;

    return true;
}

//==========================================================================================================================================
Header::Header(ContentReadBackend& reader)
{
    InputBuffer buffer;
    reader.read(buffer, HEADER_SIZE);

    std::vector<uint8_t> read_magic(6,0);
    Packager::pop_front(buffer, read_magic);
    if (!checkMagic(read_magic, 0))
        throw PartsException("Not parts archive: " + reader.source());

    m_magic = magic;

    std::vector<uint8_t> dummy(14,0);
    Packager::pop_front(buffer, dummy);
    for(size_t tmp = 0; tmp != 14; ++tmp)
        if (dummy[tmp] != 0)
            throw PartsException("Not parts archive: " + reader.source());

    Packager::pop_front(buffer, m_version);
    if (m_version != 1)
        throw PartsException("Unknown archive version: " + std::to_string(m_version));

    uint8_t tmp;
    Packager::pop_front(buffer, tmp);
    m_tocCompressionType = static_cast<CompressionType>(tmp);

    Packager::pop_front(buffer, tmp);
    m_fileCompressionType = static_cast<CompressionType>(tmp);

    Packager::pop_front(buffer, tmp);
    m_hashType = static_cast<HashType>(tmp);

    Packager::pop_front(buffer, m_tocSize);
    Packager::pop_front(buffer, m_decompressedTocSize);
}

//==========================================================================================================================================
std::vector<uint8_t> Header::getRaw() const
{
    std::vector<uint8_t> result;
    result.resize(HEADER_SIZE);
    result.clear();

    for(int tmp = 0; tmp != 6; ++tmp)
        Packager::append(result, static_cast<uint8_t>(m_magic[tmp]));

    uint8_t dummy = 0;
    for(int tmp = 0; tmp != 14; ++tmp)
        Packager::append(result, dummy);

    Packager::append(result, m_version);
    Packager::append(result, static_cast<uint8_t>(m_tocCompressionType));
    Packager::append(result, static_cast<uint8_t>(m_fileCompressionType));
    Packager::append(result, static_cast<uint8_t>(m_hashType));
    Packager::append(result, m_tocSize);
    Packager::append(result, m_decompressedTocSize);

    return result;
}

//==========================================================================================================================================
void Header::setTocSize(uint32_t size)
{
    m_tocSize = size;
}

//==========================================================================================================================================
void Header::setDecompressedTocSize(uint32_t size)
{
    m_decompressedTocSize = size;
}

