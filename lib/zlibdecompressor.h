#ifndef ZLIBDECOMPRESSOR_H
#define ZLIBDECOMPRESSOR_H

#include "decompressor.h"

namespace parts
{

class ZLibDecompressor : public Decompressor
{
public:
    ZLibDecompressor() = default;
    ~ZLibDecompressor() override = default;

    parts::InputBuffer extractBuffer(const std::vector<uint8_t>& buffer) override;

    void extractFile(const std::filesystem::path& file,
                     parts::ContentReadBackend& backend,
                     size_t position,
                     size_t compressed_size) override;
};

}
#endif // ZLIBDECOMPRESSOR_H
