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
    m_tocSize(0)
{
    m_dummy[0] = m_dummy[1] = 0;
}

//==========================================================================================================================================
Header::Header(ContentReadBackend& reader)
{
    std::vector<uint8_t> read_magic(6,0);
    reader.read(read_magic);
    for(size_t tmp = 0; tmp != 6; ++tmp)
        if (magic[tmp] != read_magic[tmp])
            throw PartsException("Not parts archive: " + reader.source());

    m_magic = magic;
    reader.read(m_version);
    if (m_version != 1)
        throw PartsException("Unknown archive version: " + std::to_string(m_version));

    uint8_t tmp;
    reader.read(tmp);
    m_tocCompressionType = static_cast<CompressionType>(tmp);

    reader.read(tmp);
    m_fileCompressionType = static_cast<CompressionType>(tmp);

    reader.read(tmp);
    m_hashType = static_cast<HashType>(tmp);

    reader.read(m_dummy[0]);
    reader.read(m_dummy[1]);

    reader.read(m_tocSize);

}

//==========================================================================================================================================
std::vector<uint8_t> Header::getRaw() const
{
    std::vector<uint8_t> result;
    result.resize(16);
    result.clear();

    for(int tmp = 0; tmp != 6; ++tmp)
        Packager::append(result, static_cast<uint8_t>(m_magic[tmp]));

    Packager::append(result, m_version);
    Packager::append(result, static_cast<uint8_t>(m_tocCompressionType));
    Packager::append(result, static_cast<uint8_t>(m_fileCompressionType));
    Packager::append(result, static_cast<uint8_t>(m_hashType));
    Packager::append(result, m_dummy[0]);
    Packager::append(result, m_dummy[1]);
    Packager::append(result, m_tocSize);

    return result;
}

//==========================================================================================================================================
void Header::setTocSize(uint32_t size)
{
    m_tocSize = size;
}

