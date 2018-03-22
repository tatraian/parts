#ifndef HASH_H
#define HASH_H

#include "parts_definitions.h"

#include "inputbuffer.h"
#include <vector>

#include <boost/filesystem/path.hpp>

namespace parts
{

class Hash
{
public:
    Hash(HashType type, const boost::filesystem::path& path);
    Hash(HashType type, const std::vector<uint8_t>& data);
    Hash(HashType type, InputBuffer& data);
    Hash(HashType type) : m_type(type) {}

    HashType type() const
    { return m_type; }

    const std::vector<uint8_t>& hash() const
    { return m_hash; }

    void setHash(const std::vector<uint8_t> & hash)
    { m_hash = hash; }

    std::string hashString() const;

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
