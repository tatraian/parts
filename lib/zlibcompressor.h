#ifndef ZLIBCOMPRESSOR_H
#define ZLIBCOMPRESSOR_H

#include "compressor.h"

namespace parts
{

class ZLibCompressor : public Compressor
{
public:
    ZLibCompressor() = default;
    ~ZLibCompressor() override = default;

    virtual size_t compressFile(const std::filesystem::path& path, ContentWriteBackend& backend) override;

    virtual size_t compressBuffer(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& backend) override;
};

}
#endif // ZLIBDECOMPRESSOR_H
