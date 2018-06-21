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
    auto ibuf = std::make_unique<uint8_t[]>(MB);
    auto obuf = std::make_unique<uint8_t[]>(MB);

    context.zalloc = Z_NULL;
    context.zfree = Z_NULL;
    context.opaque = Z_NULL;
    context.next_in = nullptr;
    context.avail_in = 0;
    context.next_out = obuf.get();
    context.avail_out = MB;

    if (inflateInit(&context)) {
        throw PartsException("Zlib decompression setup error");
    }

    int action = Z_NO_FLUSH;

    for(;;){
        if (context.avail_in == 0) {
            size_t read_bytes = read(ibuf.get(), MB);
            context.next_in = ibuf.get();
            context.avail_in = read_bytes;

            if (read_bytes != MB) {
                action = Z_FINISH;
            }
        }

        int result = inflate(&context, action);

        if (context.avail_out == 0 || result == Z_STREAM_END) {
            size_t bytes_to_write = MB - context.avail_out;
            write(obuf.get(), bytes_to_write);
            context.next_out = obuf.get();
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
parts::InputBuffer ZLibDecompressor::extractBuffer(const std::vector<uint8_t>& buffer)
{
    z_stream context;
    SimpleGuard< std::function<void()> > guard([&](){inflateEnd(&context);});

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
void ZLibDecompressor::extractFile(const boost::filesystem::path& file,
                                   parts::ContentReadBackend& backend,
                                   size_t position,
                                   size_t compressed_size)
{
    z_stream context;
    SimpleGuard< std::function<void()> > guard([&](){inflateEnd(&context);});

    LOG_DEBUG("Seeking position: {}", position);
    backend.seek(position);

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
