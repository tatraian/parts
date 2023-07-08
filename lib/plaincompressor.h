#ifndef PLAINCOMPRESSOR_H
#define PLAINCOMPRESSOR_H

#include "compressor.h"

namespace parts
{

class PlainCompressor : public Compressor
{
public:
    PlainCompressor() = default;
    ~PlainCompressor() override = default;

    virtual size_t compressFile(const std::filesystem::path& path, ContentWriteBackend& backend) override;

    virtual size_t compressBuffer(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& backend) override;
};

}
#endif // PLAINCOMPRESSOR_H
