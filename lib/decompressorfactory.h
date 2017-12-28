#ifndef DECOMPRESSORFACTORY_H
#define DECOMPRESSORFACTORY_H

#include <memory>

#include "decompressor.h"
#include "parts_definitions.h"

namespace parts
{

class DecompressorFactory
{
public:
    static std::unique_ptr<Decompressor> createDecompressor(CompressionType type);

private:
    DecompressorFactory() = delete;
    DecompressorFactory(const DecompressorFactory&) = delete;
    DecompressorFactory& operator=(const DecompressorFactory&) = delete;
    DecompressorFactory(DecompressorFactory&&) = delete;
    DecompressorFactory& operator=(DecompressorFactory&&) = delete;
};

}

#endif // DECOMPRESSORFACTORY_H
