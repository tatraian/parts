#include "decompressorfactory.h"

using namespace parts;


//==========================================================================================================================================
std::unique_ptr<Decompressor> DecompressorFactory::createDecompressor(CompressionType type)
{
    //if (type == CompressionType::LZMA) {
        //return new
    //}
    throw PartsException("Not implemented!");
}
