#ifndef LZMACOMPRESSOR_H
#define LZMACOMPRESSOR_H

#include "compressor.h"
#include "parts_definitions.h"

namespace parts
{

class LzmaCompressor : public Compressor
{
public:
    LzmaCompressor(const LzmaCompressorParameters& parameters);
    ~LzmaCompressor() override;

    size_t compressFile(const boost::filesystem::path& path, ContentWriteBackend& backend) override;

    size_t compressBuffer(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& backend) override;

protected:
    LzmaCompressorParameters m_parameters;
};

}
#endif // LZMACOMPRESSOR_H
