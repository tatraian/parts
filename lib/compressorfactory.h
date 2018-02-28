#ifndef COMPRESSORFACTORY_H
#define COMPRESSORFACTORY_H

#include <memory>

#include "parts_definitions.h"
#include "compressor.h"

namespace parts
{

class CompressorFactory
{
public:
    static std::unique_ptr<Compressor> createCompressor(CompressionType type, const PartsCompressionParameters& parameters);

private:
    CompressorFactory() = delete;
    ~CompressorFactory() = delete;

    CompressorFactory(const CompressorFactory&) = delete;
    CompressorFactory& operator=(const CompressorFactory&) = delete;

    CompressorFactory(CompressorFactory&&) = delete;
    CompressorFactory& operator=(CompressorFactory&&) = delete;
};

}

#endif // COMPRESSORFACTORY_H
