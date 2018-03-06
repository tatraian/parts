#ifndef LINKENTRY_H
#define LINKENTRY_H

#include "baseentry.h"

namespace parts
{

class LinkEntry : public BaseEntry
{
public:
    LinkEntry(const boost::filesystem::path& file,
              uint16_t permissions,
              const std::string& owner,
              uint16_t owner_id,
              const std::string& group,
              uint16_t group_id,
              const boost::filesystem::path& destination,
              bool absolute = false);

    LinkEntry(InputBuffer& buffer,
              const std::vector<std::string>& owners,
              const std::vector<std::string>& groups);

    ~LinkEntry() override = default;

    void append(std::vector<uint8_t> &buffer) const override;

    void compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend) override;

    void extractEntry(const boost::filesystem::path& dest_root, Decompressor& decompressor, ContentReadBackend& backend) override;

    void updateEntry(const BaseEntry* old_entry,
                     const boost::filesystem::path& old_root,
                     const boost::filesystem::path& dest_root,
                     Decompressor& decompressor,
                     ContentReadBackend& backend) override;

    std::string listEntry(size_t user_width, size_t size_width, std::tm* t) const override;

    std::string toString() const override;

    const boost::filesystem::path& destination() const
    { return m_destination; }

    bool absolute() const
    { return m_absolute; }

protected:
    boost::filesystem::path m_destination;
    /// TODO: should remove this...
    bool m_absolute;
};

}
#endif // LINKENTRY_H
