#ifndef LZMACOMPRESSOR_H
#define LZMACOMPRESSOR_H

#include "compressor.h"

#include "lzma.h"

namespace parts
{

struct LzmaCompressorParameters
{
    LzmaCompressorParameters(uint32_t compression_level = LZMA_PRESET_DEFAULT,
                             bool x86Filter = true) :
        m_compressionLevel(compression_level),
        m_x86FilterActive(x86Filter)
    {}

    uint32_t m_compressionLevel;
    bool m_x86FilterActive;
    // currently threads are not supported since stream_encoder_mt is not in hunter base package (cannot be compiled...)
    //uint32_t m_threads;
};

class LzmaCompressor : public Compressor
{
public:
    LzmaCompressor(const LzmaCompressorParameters& parameters);
    ~LzmaCompressor() override;

    size_t compressFile(const boost::filesystem::path& path, ContentWriteBackend& backend) override;

    size_t compressBuffer(const std::vector<uint8_t>& buffer, ContentWriteBackend& backend) override;

protected:
    void setupXZLib(lzma_stream& context);

protected:
    LzmaCompressorParameters m_parameters;
};

}
#endif // LZMACOMPRESSOR_H
