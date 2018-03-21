#ifndef ZLIBDECOMPRESSOR_H
#define ZLIBDECOMPRESSOR_H

#include "decompressor.h"

namespace parts
{

class ZlibDecompressor : public Decompressor
{
public:
    ZlibDecompressor() = default;
    ~ZlibDecompressor() override = default;

    parts::InputBuffer extractBuffer(const std::vector<uint8_t>& buffer, size_t decompressed_size) override;

    void extractFile(const boost::filesystem::path& file,
                     parts::ContentReadBackend& backend,
                     size_t position,
                     size_t compressed_size,
                     size_t decompressed_size) override;
};

}
#endif // ZLIBDECOMPRESSOR_H
