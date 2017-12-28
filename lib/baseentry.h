#ifndef BASEENTRY_H
#define BASEENTRY_H

#include "compressor.h"
#include "contentwritebackend.h"
#include "contentreadbackend.h"

#include <string>
#include <vector>
#include <deque>

#include <boost/filesystem/path.hpp>

namespace parts
{

class BaseEntry
{
public:
    BaseEntry(const boost::filesystem::path& file,
              uint16_t permissions,
              const std::string& owner,
              uint16_t owner_id,
              const std::string& group,
              uint16_t group_id) :
        m_file(file),
        m_permissions(permissions),
        m_owner(owner),
        m_ownerId(owner_id),
        m_group(group),
        m_groupId(group_id)
    {}

    BaseEntry(std::deque<uint8_t>& buffer,
              const std::vector<std::string>& owners,
              const std::vector<std::string>& groups);

    virtual ~BaseEntry() {}

    virtual void append(std::vector<uint8_t>& buffer) const;

    virtual void compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend) = 0;

    const boost::filesystem::path& file() const
    { return m_file; }

    uint16_t permissions() const
    { return m_permissions; }

    const std::string& owner() const
    { return m_owner; }

    uint16_t ownerId() const
    { return m_ownerId; }

    const std::string& group() const
    { return m_group; }

    uint16_t groupId() const
    { return m_groupId; }

protected:
    boost::filesystem::path m_file;
    uint16_t m_permissions;
    std::string m_owner;
    uint16_t m_ownerId;
    std::string m_group;
    uint16_t m_groupId;
};

}

#endif // BASEENTRY_H
