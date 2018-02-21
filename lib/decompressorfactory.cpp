#include "decompressorfactory.h"
#include "lzmadecompressor.h"

using namespace parts;


//==========================================================================================================================================
std::unique_ptr<Decompressor> DecompressorFactory::createDecompressor(CompressionType type)
{
    if (type == CompressionType::LZMA) {
        return std::unique_ptr<Decompressor>(new LzmaDecompressor());
    }
    throw PartsException("Not implemented!");
}
