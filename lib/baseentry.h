#ifndef BASEENTRY_H
#define BASEENTRY_H

#include "contentwritebackend.h"
#include "contentreadbackend.h"

#include <string>
#include <vector>
#include "inputbuffer.h"

#include <filesystem>

#include <sys/stat.h>

namespace parts
{

class BaseEntry
{
public:
    BaseEntry(const std::filesystem::path& root,
              const std ::filesystem::path& file,
              std::vector<std::string>& owners,
              std::vector<std::string>& groups,
              bool save_owner) noexcept;

    BaseEntry(InputBuffer& buffer,
              const std::vector<std::string>& owners,
              const std::vector<std::string>& groups) noexcept;

    virtual ~BaseEntry() noexcept;

    BaseEntry(const BaseEntry&) = default;
    BaseEntry& operator=(const BaseEntry&) = default;

    BaseEntry(BaseEntry&&) = default;
    BaseEntry& operator=(BaseEntry&&) = default;

    virtual void append(std::vector<uint8_t>& buffer) const;

    virtual void compressEntry(ContentWriteBackend& backend) = 0;

    virtual void extractEntry(const std::filesystem::path& dest_root, ContentReadBackend& backend, bool cont) = 0;

    virtual void updateEntry(const BaseEntry* old_entry,
                             const std::filesystem::path& old_root,
                             const std::filesystem::path& dest_root,
                             ContentReadBackend& backend,
                             bool cont) = 0;

    virtual std::string listEntry(size_t user_width, size_t size_width, std::tm* t) const = 0;

    virtual bool extractToMc(const std::filesystem::path& dest_file, ContentReadBackend& backend)
    { return false; }

    const std::filesystem::path& file() const
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

    operator bool() const
    { return m_valid; }

protected:
    // Constructor for unit tests only
    BaseEntry(const std::filesystem::path& root,
              const std::filesystem::path& file,
              uint16_t permissions,
              const std::string& owner,
              uint16_t owner_id,
              const std::string& group,
              uint16_t group_id) noexcept:
        m_valid(true),
        m_root(root),
        m_file(file),
        m_permissions(permissions),
        m_owner(owner),
        m_ownerId(owner_id),
        m_group(group),
        m_groupId(group_id)
    {}

    void setMetadata(const std::filesystem::path& dest_root);

    std::string permissionsToString() const;

    uint16_t getOwnerId(const struct stat* file_stat, std::vector<std::string>& owners, bool save);
    uint16_t getGroupId(const struct stat* file_stat, std::vector<std::string>& groups, bool save);
    uint16_t getPermissions(const struct stat* file_stat);

    uint16_t findOrInsert(const std::string& name, std::vector<std::string>& table);

protected:
    bool m_valid;
    std::filesystem::path m_root;

    std::filesystem::path m_file;
    uint16_t m_permissions;
    std::string m_owner;
    uint16_t m_ownerId;
    std::string m_group;
    uint16_t m_groupId;
};

}

#endif // BASEENTRY_H
