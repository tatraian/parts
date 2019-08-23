#ifndef DIRECTORYENTRY_H
#define DIRECTORYENTRY_H

#include "baseentry.h"

namespace parts
{

class DirectoryEntry : public BaseEntry
{
public:
    DirectoryEntry(const boost::filesystem::path& root,
                   const boost::filesystem::path& file,
                   std::vector<std::string>& owners,
                   std::vector<std::string>& groups,
                   bool save_owner) noexcept;

    DirectoryEntry(InputBuffer& buffer,
                   const std::vector<std::string>& owners,
                   const std::vector<std::string>& groups) noexcept;

    ~DirectoryEntry() noexcept override = default;

    DirectoryEntry(const DirectoryEntry&) = default;
    DirectoryEntry& operator=(const DirectoryEntry&) = default;

    DirectoryEntry(DirectoryEntry&&) = default;
    DirectoryEntry& operator=(DirectoryEntry&&) = default;

    void append(std::vector<uint8_t>& buffer) const override;

    void compressEntry(ContentWriteBackend& backend) override;

    void extractEntry(const boost::filesystem::path& dest_root, ContentReadBackend& backend, bool cont) override;

    void updateEntry(const BaseEntry* old_entry,
                     const boost::filesystem::path& old_root,
                     const boost::filesystem::path& dest_root,
                     ContentReadBackend& backend,
                     bool cont) override;

    std::string listEntry(size_t user_width, size_t size_width, std::tm* t) const override;

    std::string toString() const override;

protected:
    // Constructor for unit tests:
    DirectoryEntry(const boost::filesystem::path& root,
                   const boost::filesystem::path& file,
                   uint16_t permissions,
                   const std::string& owner,
                   uint16_t owner_id,
                   const std::string& group,
                   uint16_t group_id) :
        BaseEntry(root, file, permissions, owner, owner_id, group, group_id)
    {}

};

}

#endif // DIRECTORYENTRY_H
