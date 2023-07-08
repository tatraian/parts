#include <zlib.h>
#include <fstream>
#include <functional>

#include "simpleguard.h"
#include "parts_definitions.h"
#include "internal_definitions.h"

#include "zlibcompressor.h"

using namespace parts;

//==========================================================================================================================================
size_t ZLibCompressor::compressFile(const std::filesystem::path& path, ContentWriteBackend& backend)
{
    z_stream context;
    auto guard = SimpleGuard<std::function<void()>>([&](){deflateEnd(&context); });
    size_t compressed_size = 0;

    std::ifstream input_file(path.string(), std::ios::binary | std::ios::in);
    if (!input_file)
        throw PartsException("Cannot find file to compress: " + path.string());

    std::vector<uint8_t> output(MB,0);
    std::vector<uint8_t> input(MB,0);

    context.zalloc = Z_NULL;
    context.zfree = Z_NULL;
    context.opaque = Z_NULL;
    context.next_in = nullptr;
    context.avail_in = 0;
    context.next_out = &output[0];
    context.avail_out = MB;

    if (deflateInit(&context, Z_BEST_COMPRESSION)) {
        throw PartsException("Zlib compression setup error");
    }

    int action = Z_NO_FLUSH;
    for(;;) {
        if (context.avail_in == 0 && !input_file.eof()) {
            input_file.read(reinterpret_cast<char*>(&input[0]), MB);

            if (input_file.bad())
                throw PartsException("Error during reading file: " + path.string());

            context.next_in = &input[0];
            context.avail_in = input_file.gcount();

            if (input_file.eof())
                action = Z_FINISH;
        }

        int result = deflate(&context, action);

        if (context.avail_out == 0 || result == Z_STREAM_END) {
            size_t real_size = MB - context.avail_out;
            output.resize(real_size);
            compressed_size += real_size;
            backend.append(output);
            output.resize(MB);

            context.next_out = &output[0];
            context.avail_out = MB;
        }

        if (result == Z_OK)
            continue;
        else if (result == Z_STREAM_END)
            break;
        else
            throw PartsException("Unknown compression error during file: " + path.string());
    }

    return compressed_size;
}

//==========================================================================================================================================
size_t ZLibCompressor::compressBuffer(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& backend)
{
    z_stream context;
    auto guard = SimpleGuard<std::function<void()>>([&](){deflateEnd(&context); });
    size_t compressed_size = 0;

    std::vector<uint8_t> output(MB,0);

    context.zalloc = Z_NULL;
    context.zfree = Z_NULL;
    context.opaque = Z_NULL;
    context.next_in = const_cast<uint8_t *>(&buffer[0]);
    context.avail_in = buffer.size();
    context.next_out = &output[0];
    context.avail_out = MB;

    if (deflateInit(&context, Z_BEST_COMPRESSION)) {
        throw PartsException("Zlib compression setup error");
    }

    for(;;) {
        int result = deflate(&context, Z_FINISH);
        if (context.avail_out == 0 || result == Z_STREAM_END) {
            size_t real_size = MB - context.avail_out;
            output.resize(real_size);
            compressed_size += real_size;
            backend.insert(backend.end(), output.begin(), output.end());
            output.resize(MB);
            context.next_out = &output[0];
            context.avail_out = MB;
        }

        if (result == Z_OK)
            continue;
        else if (result == Z_STREAM_END)
            break;
        else
            throw PartsException("Unknown compression error");
    }

    return compressed_size;
}
