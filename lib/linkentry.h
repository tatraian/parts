#ifndef LINKENTRY_H
#define LINKENTRY_H

#include "baseentry.h"

namespace parts
{

class LinkEntry : public BaseEntry
{
public:
    LinkEntry(const boost::filesystem::path& root,
              const boost::filesystem::path& file,
              std::vector<std::string>& owners,
              std::vector<std::string>& groups,
              bool save_owner) noexcept;

    LinkEntry(InputBuffer& buffer,
              const std::vector<std::string>& owners,
              const std::vector<std::string>& groups) noexcept;

    ~LinkEntry() noexcept override = default;

    LinkEntry(const LinkEntry&) = default;
    LinkEntry& operator=(const LinkEntry&) = default;

    LinkEntry(LinkEntry&&) = default;
    LinkEntry& operator=(LinkEntry&&) = default;

    void append(std::vector<uint8_t> &buffer) const override;

    void compressEntry(ContentWriteBackend& backend) override;

    void extractEntry(const boost::filesystem::path& dest_root, ContentReadBackend& backend, bool cont) override;

    void updateEntry(const BaseEntry* old_entry,
                     const boost::filesystem::path& old_root,
                     const boost::filesystem::path& dest_root,
                     ContentReadBackend& backend,
                     bool cont) override;

    std::string listEntry(size_t user_width, size_t size_width, std::tm* t) const override;

    std::string toString() const override;

    const boost::filesystem::path& destination() const
    { return m_destination; }

protected:
    // Constructor for only unit tests
    LinkEntry(const boost::filesystem::path& root,
              const boost::filesystem::path& file,
              uint16_t permissions,
              const std::string& owner,
              uint16_t owner_id,
              const std::string& group,
              uint16_t group_id,
              const boost::filesystem::path& destination,
              bool absolute) :
        BaseEntry(root, file, permissions, owner, owner_id, group, group_id),
        m_destination(destination)
    {}

    static bool fileInsideRoot(const boost::filesystem::path& root, const boost::filesystem::path& file);

protected:
    boost::filesystem::path m_destination;
};

}
#endif // LINKENTRY_H
