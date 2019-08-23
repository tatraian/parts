#include "header.h"

#include "packager.h"

#include "logger_internal.h"
using namespace parts;

namespace {
char magic[] = {'p','a','r','t','s','!'};
}

//==========================================================================================================================================
Header::Header(const PartsCompressionParameters& parameters) noexcept:
    m_valid(true),
    m_magic(magic),
    m_version(2),
    m_tocCompressionType(parameters.m_tocCompression),
    m_hashType(parameters.m_hashType),
    m_tocSize(0)
{
    m_dummy[0] = m_dummy[1] = 0;
}

//==========================================================================================================================================
Header::Header(ContentReadBackend& reader) noexcept :
    m_valid(false)
{
    InputBuffer buffer;
    reader.read(buffer, 16);

    std::vector<uint8_t> read_magic(6,0);
    Packager::pop_front(buffer, read_magic);
    for(size_t tmp = 0; tmp != 6; ++tmp)
        if (magic[tmp] != read_magic[tmp]) {
            LOG_ERROR("Not parts archive: {}", reader.source());
            m_valid = false;
        }

    m_magic = magic;
    Packager::pop_front(buffer, m_version);
    if (m_version == 1) {
        LOG_ERROR("Unhandled version: {}", m_version);
        return;
    }

    if (m_version != 2) {
        LOG_ERROR("Unknown archive version: {}", m_version);
        return;
    }

    try {
        uint8_t tmp;
        Packager::pop_front(buffer, tmp);
        m_tocCompressionType = static_cast<CompressionType>(tmp);

        Packager::pop_front(buffer, tmp);
        m_hashType = static_cast<HashType>(tmp);

        Packager::pop_front(buffer, m_dummy[0]);
        Packager::pop_front(buffer, m_dummy[1]);
        Packager::pop_front(buffer, m_dummy[2]);

        Packager::pop_front(buffer, m_tocSize);
    } catch (const std::exception&) {
        LOG_ERROR("There is no enough data for reading header!");
        return;
    }
    m_valid = true;
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
    Packager::append(result, static_cast<uint8_t>(m_hashType));
    Packager::append(result, m_dummy[0]);
    Packager::append(result, m_dummy[1]);
    Packager::append(result, m_dummy[2]);
    Packager::append(result, m_tocSize);

    return result;
}

//==========================================================================================================================================
void Header::setTocSize(uint32_t size)
{
    m_tocSize = size;
}


