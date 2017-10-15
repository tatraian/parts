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

    ~DirectoryEntry() override = default;

    void append(std::deque<uint8_t>& buffer) const override;

    void compressEntry(Compressor& compressor, ContentWriteBackend& backend) override;
};

}

#endif // DIRECTORYENTRY_H