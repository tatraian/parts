#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include "inputbuffer.h"
#include <vector>

#include "contentreadbackend.h"
#include <boost/filesystem/path.hpp>


namespace parts
{

class Decompressor {
public:
    Decompressor() = default;
    Decompressor(const Decompressor&) = delete;
    Decompressor& operator=(const Decompressor&) = delete;

    Decompressor(Decompressor&&) = delete;
    Decompressor& operator=(Decompressor&&) = delete;

    virtual ~Decompressor() = default;

    virtual InputBuffer extractBuffer(const std::vector<uint8_t>& buffer, size_t decompressed_size) = 0;

    virtual void extractFile(const boost::filesystem::path& file,
                             ContentReadBackend& backend,
                             size_t position,
                             size_t compressed_size,
                             size_t decompressed_size) = 0;
};

}


#endif // DECOMPRESSOR_H
