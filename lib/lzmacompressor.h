#ifndef LZMACOMPRESSOR_H
#define LZMACOMPRESSOR_H

#include "compressor.h"

namespace parts
{

struct LzmaCompressorParameters
{
    size_t m_threads;
    bool m_x86FilterActive;
    size_t m_compressionLevel;
};

class LzmaCompressor : public Compressor
{
public:
    LzmaCompressor(const LzmaCompressorParameters& parameters);
    ~LzmaCompressor() override;

    size_t compressFile(const boost::filesystem::path& path, ContentWriteBackend& backend) override;

    size_t compressBuffer(const std::vector<uint8_t>& buffer, ContentWriteBackend& backend) override;

protected:
    LzmaCompressorParameters m_parameters;
};

}
#endif // LZMACOMPRESSOR_H
