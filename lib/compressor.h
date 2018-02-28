#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "contentwritebackend.h"

#include "inputbuffer.h"
#include <boost/filesystem/path.hpp>


namespace parts
{

class Compressor {
public:
    virtual ~Compressor() = default;

    /**
     * @brief compressFile
     * @param path The path of the file to be compressed
     * @param backend The writer backend into which the compressed data will be written.
     * @return the compressed content size
     */
    virtual size_t compressFile(const boost::filesystem::path& path,
                                ContentWriteBackend& backend) = 0;

    virtual size_t compressBuffer(const std::vector<uint8_t>& buffer,
                                  std::vector<uint8_t>& backend) = 0;
};

}

#endif // COMPRESSOR_H
