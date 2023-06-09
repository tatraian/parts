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

    InputBuffer extractBuffer(const std::vector<uint8_t>& buffer) override;

    void extractFile(const std::filesystem::path& file,
                     ContentReadBackend& backend,
                     size_t position,
                     size_t compressed_size) override;
};

}

#endif // PLAINDECOMPRESSOR_H
