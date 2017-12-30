#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <deque>
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

    virtual std::deque<uint8_t> extractBuffer(const std::vector<uint8_t>& buffer) = 0;

    virtual void extractFile(const boost::filesystem::path& file,
                             ContentReadBackend& backend,
                             size_t position,
                             size_t compressed_size) = 0;
};

}


#endif // DECOMPRESSOR_H
