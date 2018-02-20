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

    LinkEntry(std::deque<uint8_t>& buffer,
              const std::vector<std::string>& owners,
              const std::vector<std::string>& groups);

    ~LinkEntry() override = default;

    void append(std::vector<uint8_t> &buffer) const override;

    void compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend) override;

    void extractEntry(const boost::filesystem::path& dest_root, Decompressor& decompressor, ContentReadBackend& backend) override;

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
