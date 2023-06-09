#ifndef LZMADECOMPRESSOR_H
#define LZMADECOMPRESSOR_H

#include "decompressor.h"

namespace parts
{

class LzmaDecompressor : public Decompressor
{
public:
    LzmaDecompressor() = default;
    ~LzmaDecompressor() override = default;

    InputBuffer extractBuffer(const std::vector<uint8_t>& buffer) override;

    void extractFile(const std::filesystem::path& file,
                     ContentReadBackend& backend,
                     size_t position,
                     size_t compressed_size) override;
};

}
#endif // LZMADECOMPRESSOR_H
