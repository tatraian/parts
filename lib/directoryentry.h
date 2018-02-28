#ifndef DIRECTORYENTRY_H
#define DIRECTORYENTRY_H

#include "baseentry.h"

namespace parts
{

class DirectoryEntry : public BaseEntry
{
public:
    DirectoryEntry(const boost::filesystem::path& file,
                   uint16_t permissions,
                   const std::string& owner,
                   uint16_t owner_id,
                   const std::string& group,
                   uint16_t group_id);

    DirectoryEntry(InputBuffer& buffer,
                   const std::vector<std::string>& owners,
                   const std::vector<std::string>& groups);

    ~DirectoryEntry() override = default;

    void append(std::vector<uint8_t>& buffer) const override;

    void compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend) override;

    void extractEntry(const boost::filesystem::path& dest_root, Decompressor& decompressor, ContentReadBackend& backend) override;

    void updateEntry(const BaseEntry* old_entry,
                     const boost::filesystem::path& old_root,
                     const boost::filesystem::path& dest_root,
                     Decompressor& decompressor,
                     ContentReadBackend& backend) override;

    std::string toString() const override;
};

}

#endif // DIRECTORYENTRY_H
