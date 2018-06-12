#include "hash.h"

#include <fstream>

#include <boost/filesystem.hpp>

#include <digestpp/algorithm/md5.hpp>
#include <digestpp/algorithm/sha2.hpp>
#include <digestpp/hasher.hpp>

using namespace parts;

//==========================================================================================================================================
Hash::Hash() :
    m_type(HashType::MD5)
{}

//==========================================================================================================================================
Hash::Hash(HashType type, const boost::filesystem::path& path) :
    m_type(type),
    m_hash(hash_size(m_type), 0)
{
    if (!boost::filesystem::exists(path))
        throw PartsException("File doesn't exist: " + path.string());

    if (!boost::filesystem::is_regular_file(path))
        throw PartsException("File is not regular file: " + path.string());

    std::ifstream file(path.string(), std::ios::binary);
    switch (type) {
        case HashType::MD5:
            digestpp::md5().absorb(file).digest(&m_hash[0], m_hash.size());
            break;
        case HashType::SHA256:
            digestpp::sha256().absorb(file).digest(&m_hash[0], m_hash.size());
            break;
    }
}

//==========================================================================================================================================
Hash::Hash(HashType type, const std::vector<uint8_t>& data) :
    m_type(type),
    m_hash(hash_size(m_type), 0)
{
    switch (type) {
        case HashType::MD5:
            digestpp::md5().absorb(data.begin(), data.end()).digest(&m_hash[0], m_hash.size());
            break;
        case HashType::SHA256:
            digestpp::sha256().absorb(data.begin(), data.end()).digest(&m_hash[0], m_hash.size());
            break;
    }
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
    std::stringstream output;
    output.setf(std::ios::hex, std::ios::basefield);
    for(const uint8_t& value : m_hash) {
        output.width(2);
        output.fill('0');
        int tmp = value;
        output << tmp;
    }

    return output.str();
}
