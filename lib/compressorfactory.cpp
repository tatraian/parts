#include <boost/lexical_cast.hpp>
#include "compressorfactory.h"

#include "plaincompressor.h"
#include "lzmacompressor.h"

using namespace parts;

//==========================================================================================================================================
std::unique_ptr<Compressor> CompressorFactory::createCompressor(CompressionType type, const PartsCompressionParameters& parameters)
{
    switch (type) {
        case CompressionType::None:
            return std::unique_ptr<Compressor>(new PlainCompressor());
        case CompressionType::LZMA:
            return std::unique_ptr<Compressor>(new LzmaCompressor(parameters.m_lzmaParameters));
        case CompressionType::EXTERNAL:
            throw PartsException("Cannot use external compressor internally");
        default:
            throw PartsException("Invalid compressor :"+boost::lexical_cast<std::string>((uint32_t)type));
    }
}
