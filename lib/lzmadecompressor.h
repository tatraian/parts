#ifndef LZMADECOMPRESSOR_H
#define LZMADECOMPRESSOR_H

#include "decompressor.h"
#include "lzma.h"

namespace parts
{

class LzmaDecompressor : public Decompressor
{
public:
    LzmaDecompressor() = default;
    ~LzmaDecompressor() override = default;

    InputBuffer extractBuffer(const std::vector<uint8_t>& buffer) override;

    void extractFile(const boost::filesystem::path& file,
                     ContentReadBackend& backend,
                     size_t position,
                     size_t compressed_size) override;
protected:
    void setupXZLib(lzma_stream& lzma_context);

    void extractInternal(lzma_stream& lzma_context,
                         std::function<size_t(uint8_t*, size_t)> read,
                         std::function<void(uint8_t*, size_t)> write);
};

}
#endif // LZMADECOMPRESSOR_H
