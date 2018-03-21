#include <boost/lexical_cast.hpp>
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
            return std::unique_ptr<Decompressor>(new ZlibDecompressor());
        default:
            throw PartsException("Invalid decompressor :"+boost::lexical_cast<std::string>((uint32_t)type));
    }
}
