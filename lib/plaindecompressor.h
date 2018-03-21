#ifndef PLAINDECOMPRESSOR_H
#define PLAINDECOMPRESSOR_H

#include "decompressor.h"

namespace parts
{

class PlainDecompressor : public Decompressor
{
public:
    PlainDecompressor() = default;
    ~PlainDecompressor() override = default;

    InputBuffer extractBuffer(const std::vector<uint8_t>& buffer, size_t decompressed_size) override;

    void extractFile(const boost::filesystem::path& file,
                     ContentReadBackend& backend,
                     size_t position,
                     size_t compressed_size,
                     size_t decompressed_size) override;
};

}

#endif // PLAINDECOMPRESSOR_H
