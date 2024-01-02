#include "decompressorfactory.h"
#include "lzmadecompressor.h"
#include "plaindecompressor.h"
#include "zlibdecompressor.h"

using namespace parts;


//==========================================================================================================================================
std::unique_ptr<Decompressor> DecompressorFactory::createDecompressor(CompressionType type)
{
    switch (type) {
    case CompressionType::None:
        return std::unique_ptr<Decompressor>(new PlainDecompressor());
    case CompressionType::LZMA:
        return std::unique_ptr<Decompressor>(new LzmaDecompressor());
    case CompressionType::ZLIB:
        return std::unique_ptr<Decompressor>(new ZLibDecompressor());
    default:
        throw PartsException(std::string("Invalid decompressor ") + to_string(type));
    }
}
