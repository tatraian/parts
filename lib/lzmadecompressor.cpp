#include "lzmadecompressor.h"

#include "parts_definitions.h"
#include "internal_definitions.h"
#include "simpleguard.h"
#include "logger.h"

#include <fstream>

using namespace parts;


//==========================================================================================================================================
InputBuffer LzmaDecompressor::extractBuffer(const std::vector<uint8_t>& buffer)
{
    lzma_stream lzma_context;
    setupXZLib(lzma_context);
    SimpleGuard< std::function<void()> > guard([&](){lzma_end(&lzma_context);});

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

    extractInternal(lzma_context, reader, writer);
    return result;
}

//==========================================================================================================================================
void LzmaDecompressor::extractFile(const boost::filesystem::path& file,
                                   ContentReadBackend& backend,
                                   size_t position,
                                   size_t compressed_size)
{
    lzma_stream lzma_context;
    setupXZLib(lzma_context);
    SimpleGuard< std::function<void()> > guard([&](){lzma_end(&lzma_context);});

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

    extractInternal(lzma_context, reader, writer);
}

//==========================================================================================================================================
void LzmaDecompressor::setupXZLib(lzma_stream& lzma_context)
{
    lzma_context = LZMA_STREAM_INIT;
    lzma_ret result = lzma_stream_decoder(&lzma_context, UINT64_MAX, LZMA_CONCATENATED);

    if (result == LZMA_OK)
        return;

    if (result == LZMA_MEM_ERROR)
        throw PartsException("Cannot allocate memory of lzma decompressor");

    throw PartsException("Unhandled error during lzma decompressor creation");
}

//==========================================================================================================================================
void LzmaDecompressor::extractInternal(lzma_stream& lzma_context,
                                       std::function<size_t (uint8_t*, size_t)> read,
                                       std::function<void (uint8_t*, size_t)> write)
{
    uint8_t ibuf[MB];
    uint8_t obuf[MB];

    lzma_context.next_in = nullptr;
    lzma_context.avail_in = 0;
    lzma_context.next_out = obuf;
    lzma_context.avail_out = MB;
    lzma_action action = LZMA_RUN;

    for(;;){
        if (lzma_context.avail_in == 0) {
            size_t read_bytes = read(ibuf, MB);
            lzma_context.next_in = ibuf;
            lzma_context.avail_in = read_bytes;

            if (read_bytes != MB) {
                action = LZMA_FINISH;
            }
        }

        lzma_ret result = lzma_code(&lzma_context, action);

        if (lzma_context.avail_out == 0 || result == LZMA_STREAM_END) {
            size_t bytes_to_write = MB - lzma_context.avail_out;

            write(obuf, bytes_to_write);
            lzma_context.next_out = obuf;
            lzma_context.avail_out = 1024*1024;
            if (result == LZMA_STREAM_END)
                break;

            continue;
        }

        if (result != LZMA_OK) {
            throw PartsException("Invalid lzam return code: " + std::to_string(result));
        }
    }
}
