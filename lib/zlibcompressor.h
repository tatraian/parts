#ifndef ZLIBCOMPRESSOR_H
#define ZLIBCOMPRESSOR_H

#include "compressor.h"

namespace parts
{

class ZlibCompressor : public Compressor
{
public:
    ZlibCompressor() = default;
    ~ZlibCompressor() override = default;

    virtual size_t compressFile(const boost::filesystem::path& path, ContentWriteBackend& backend) override;

    virtual size_t compressBuffer(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& backend) override;
};

}
#endif // ZLIBDECOMPRESSOR_H
