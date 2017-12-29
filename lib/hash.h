#ifndef HASH_H
#define HASH_H

#include "parts_definitions.h"

#include <deque>
#include <vector>

#include <boost/filesystem/path.hpp>

namespace parts
{

class Hash
{
public:
    Hash(HashType type, const boost::filesystem::path& path);
    Hash(HashType type, const std::vector<uint8_t>& data);
    Hash(HashType type, std::deque<uint8_t>& data);

    HashType type() const
    { return m_type; }

    const std::vector<uint8_t>& hash() const
    { return m_hash; }

    std::string hashString() const;

protected:
    HashType m_type;
    std::vector<uint8_t> m_hash;
};

}

#endif // HASH_H
