#include "decompressorfactory.h"
#include "lzmadecompressor.h"
#include "plaindecompressor.h"

using namespace parts;


//==========================================================================================================================================
std::unique_ptr<Decompressor> DecompressorFactory::createDecompressor(CompressionType type)
{
    switch (type) {
        case CompressionType::None:
            return std::unique_ptr<Decompressor>(new PlainDecompressor());
        case CompressionType::LZMA:
            return std::unique_ptr<Decompressor>(new LzmaDecompressor());
    }
}
