#include <fstream>
#include <zlib.h>

#include "simpleguard.h"
#include "parts_definitions.h"
#include "internal_definitions.h"
#include "logger_internal.h"

#include "zlibdecompressor.h"

using namespace parts;

namespace {

//==========================================================================================================================================
void extractInternal(z_stream& context,
                     std::function<size_t (uint8_t*, size_t)> read,
                     std::function<void (uint8_t*, size_t)> write)
{
    uint8_t ibuf[MB];
    uint8_t obuf[MB];

    context.zalloc = Z_NULL;
    context.zfree = Z_NULL;
    context.opaque = Z_NULL;
    context.next_in = nullptr;
    context.avail_in = 0;
    context.next_out = obuf;
    context.avail_out = MB;

    if (inflateInit(&context)) {
        throw PartsException("Zlib decompression setup error");
    }

    SimpleGuard< std::function<void()> > guard([&](){inflateEnd(&context);});

    int action = Z_NO_FLUSH;

    for(;;){
        if (context.avail_in == 0) {
            size_t read_bytes = read(ibuf, MB);
            context.next_in = ibuf;
            context.avail_in = read_bytes;

            if (read_bytes != MB) {
                action = Z_FINISH;
            }
        }

        int result = inflate(&context, action);

        if (context.avail_out == 0 || result == Z_STREAM_END) {
            size_t bytes_to_write = MB - context.avail_out;

            write(obuf, bytes_to_write);
            context.next_out = obuf;
            context.avail_out = 1024*1024;
            if (result == Z_STREAM_END)
                break;

            continue;
        }

        if (result != Z_OK) {
            throw PartsException("Invalid zlib return code: " + std::to_string(result));
        }
    }
}

}

//==========================================================================================================================================
parts::InputBuffer ZlibDecompressor::extractBuffer(const std::vector<uint8_t>& buffer, size_t decompressed_size)
{
    z_stream context;

    if (buffer.empty() || decompressed_size == 0)
    {
        InputBuffer result;
        return result;
    }

    size_t start = 0;
    auto reader = [&](uint8_t* dest, size_t size) {
        size_t bytes = std::min(buffer.size() - start, size);
        std::memcpy(dest, buffer.data() + start, bytes);
        start += bytes;
        return bytes;
    };

    InputBuffer result;
    auto writer = [&](uint8_t* src, size_t size) {
        result.insert(result.end(), src, src + size);
    };

    extractInternal(context, reader, writer);
    return result;
}

//==========================================================================================================================================
void ZlibDecompressor::extractFile(const boost::filesystem::path& file,
                                   parts::ContentReadBackend& backend,
                                   size_t position,
                                   size_t compressed_size,
                                   size_t decompressed_size)
{
    z_stream context;

    LOG_DEBUG("Seeking position: {}", position);
    backend.seek(position);

    if (compressed_size == 0 || decompressed_size == 0)
    {
        return;
    }

    size_t read_bytes = 0;
    auto reader = [&](uint8_t* dest, size_t size) {
        size_t bytes = std::min(compressed_size - read_bytes, size);
        backend.read(dest, bytes);
        read_bytes += bytes;
        return bytes;
    };

    std::ofstream output(file.string(), std::ios::binary | std::ios::ate);
    auto writer = [&](uint8_t* src, size_t size) {
        output.write(reinterpret_cast<char*>(src), size);
    };

    extractInternal(context, reader, writer);
}