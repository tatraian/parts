#include "hash.h"

#include <fstream>

#include <boost/filesystem.hpp>

#include <picosha2.h>

using namespace parts;

//==========================================================================================================================================
Hash::Hash(HashType type, const boost::filesystem::path& path) :
    m_type(type),
    m_hash(32)
{
    if (!boost::filesystem::exists(path))
        throw PartsException("File doesn't exist: " + path.string());

    if (!boost::filesystem::is_regular_file(path))
        throw PartsException("File is not regular file: " + path.string());

    if (type != HashType::SHA256)
        throw PartsException("Only SHA 256 hash is supported currently");

    std::ifstream file(path.string(), std::ios::binary);
    picosha2::hash256(std::istreambuf_iterator<char>(file),
                      std::istreambuf_iterator<char>(),
                      m_hash.begin(),
                      m_hash.end());
}

//==========================================================================================================================================
Hash::Hash(HashType type, const std::vector<uint8_t>& data) :
    m_type(type),
    m_hash(hash_size(m_type), 0)
{
    if (type != HashType::SHA256)
        throw PartsException("Only SHA 256 hash is supported currently");

    picosha2::hash256(data.begin(), data.end(), m_hash.begin(), m_hash.end());
}

//==========================================================================================================================================
Hash::Hash(HashType type, InputBuffer& data) :
    m_type(type),
    m_hash(hash_size(m_type), 0)
{

    m_hash.clear();

    if (data.size() < m_hash.size())
        throw PartsException("No enough data to read hash");
    m_hash.insert(m_hash.end(), data.begin(), data.begin() + hash_size(type));
    data.erase(data.begin(), data.begin() + hash_size(type));
}

//==========================================================================================================================================
std::string Hash::hashString() const
{
    switch (m_type) {
    case HashType::SHA256:
        return picosha2::bytes_to_hex_string(m_hash.begin(), m_hash.end());
    case HashType::MD5:
        break;
    }
    throw PartsException("Unsupported hash! Only SHA-256 is supported currently");
}
