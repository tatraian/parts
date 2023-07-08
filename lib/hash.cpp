#include "hash.h"

#include "logger_internal.h"

#include <fstream>

#include <filesystem>
#include <boost/algorithm/hex.hpp>

#include <digestpp/algorithm/md5.hpp>
#include <digestpp/algorithm/sha2.hpp>
#include <digestpp/hasher.hpp>

using namespace parts;

//==========================================================================================================================================
Hash::Hash() :
    m_type(HashType::MD5)
{}

//==========================================================================================================================================
Hash::Hash(HashType type, const std::filesystem::path& path) :
    m_type(type),
    m_hash(hash_size(m_type), 0)
{
    if (!std::filesystem::exists(path))
    {
        LOG_ERROR("File doesn't exist: {}", path.string());
        m_hash.clear();
    }

    if (!std::filesystem::is_regular_file(path)) {
        LOG_ERROR("File is not regular file: {}", path.string());
        m_hash.clear();
    }

    std::ifstream file(path.string(), std::ios::binary);
    try {
        switch (type) {
        case HashType::MD5:
            digestpp::md5().absorb(file).digest(&m_hash[0], m_hash.size());
            break;
        case HashType::SHA256:
            digestpp::sha256().absorb(file).digest(&m_hash[0], m_hash.size());
            break;
        }
    } catch(const std::exception& e) {
        LOG_ERROR("Cannot calculate file's hash: {}", path.string());
        m_hash.clear();
    }
}

//==========================================================================================================================================
Hash::Hash(HashType type, const std::vector<uint8_t>& data) :
    m_type(type),
    m_hash(hash_size(m_type), 0)
{
    try {

        switch (type) {
        case HashType::MD5:
            digestpp::md5().absorb(data.begin(), data.end()).digest(&m_hash[0], m_hash.size());
            break;
        case HashType::SHA256:
            digestpp::sha256().absorb(data.begin(), data.end()).digest(&m_hash[0], m_hash.size());
            break;
        }
    } catch(const std::exception& e) {
        LOG_ERROR("Cannot calculate buffer's hash!");
        m_hash.clear();
    }

}

//==========================================================================================================================================
Hash::Hash(HashType type, InputBuffer& data) :
    m_type(type),
    m_hash(hash_size(m_type), 0)
{
    if (data.size() < m_hash.size()) {
        LOG_ERROR("No enough data to read hash");
        m_hash.clear();
        return;
    }

    m_hash.clear();

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
