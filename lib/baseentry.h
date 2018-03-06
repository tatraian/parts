#ifndef BASEENTRY_H
#define BASEENTRY_H

#include "compressor.h"
#include "decompressor.h"
#include "contentwritebackend.h"
#include "contentreadbackend.h"

#include <string>
#include <vector>
#include "inputbuffer.h"

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

    BaseEntry(InputBuffer& buffer,
              const std::vector<std::string>& owners,
              const std::vector<std::string>& groups);

    virtual ~BaseEntry() {}

    virtual void append(std::vector<uint8_t>& buffer) const;

    virtual void compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend) = 0;

    virtual void extractEntry(const boost::filesystem::path& dest_root, Decompressor& decompressor, ContentReadBackend& backend) = 0;

    virtual void updateEntry(const BaseEntry* old_entry,
                             const boost::filesystem::path& old_root,
                             const boost::filesystem::path& dest_root,
                             Decompressor& decompressor,
                             ContentReadBackend& backend) = 0;

    virtual std::string listEntry(size_t user_width, size_t size_width, std::tm* t) const = 0;

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

    virtual std::string toString() const;

protected:
    void setMetadata(const boost::filesystem::path& dest_root);

    std::string permissionsToString() const;

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
