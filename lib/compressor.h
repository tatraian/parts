#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "contentwritebackend.h"

#include <boost/filesystem/path.hpp>


namespace parts
{

class Compressor {
public:
    virtual ~Compressor() = default;

    virtual void compressFile(const boost::filesystem::path& path,
                              ContentWriteBackend& backend) = 0;

    virtual void compressBuffer(const std::vector<uint8_t>& buffer,
                                ContentWriteBackend& backend) = 0;
};

}

#endif // COMPRESSOR_H
