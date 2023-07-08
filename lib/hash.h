#ifndef HASH_H
#define HASH_H

#include "parts_definitions.h"

#include "inputbuffer.h"

#include <string>
#include <vector>

#include <filesystem>
#include <boost/algorithm/hex.hpp>

namespace parts
{

class Hash
{
public:
    Hash();
    Hash(HashType type, const std::filesystem::path& path);
    Hash(HashType type, const std::vector<uint8_t>& data);
    Hash(HashType type, InputBuffer& data);
    template<class It>
    Hash(HashType type, It begin, It end)  : m_type(type) {
        boost::algorithm::unhex(begin, end, std::back_inserter(m_hash));

        if (m_hash.size() != hash_size(m_type))
            throw PartsException("The given hash lenght is not correct! (wished: " + std::to_string(hash_size(m_type)) +
                                 ", expected: " + std::to_string(m_hash.size()) + ")");
    }

    HashType type() const
    { return m_type; }

    const std::vector<uint8_t>& hash() const
    { return m_hash; }

    std::string hashString() const;

    bool isValid() const
    { return !m_hash.empty(); }

protected:
    HashType m_type;
    std::vector<uint8_t> m_hash;
};

inline bool operator==(const Hash& ls, const Hash& rs) {
    return ls.hash() == rs.hash();
}

inline bool operator!=(const Hash& ls, const Hash& rs) {
    return !(ls == rs);
}

}

#endif // HASH_H
