#include "lzmacompressor.h"

#include <fstream>
#include <lzma.h>
#include <functional>

#include "parts_definitions.h"
#include "internal_definitions.h"
#include "simpleguard.h"

using namespace parts;

namespace {

//==========================================================================================================================================
void setupXZLib(lzma_stream& context, LzmaCompressorParameters parameters)
{
    context = LZMA_STREAM_INIT;
    lzma_options_lzma lzma_options;
    if (lzma_lzma_preset(&lzma_options, parameters.m_compressionLevel))
        throw PartsException("Unsupported compression level: " + std::to_string(parameters.m_compressionLevel));

    lzma_filter filters_x86 [] = {
        { .id = LZMA_FILTER_X86, .options = nullptr },
        { .id = LZMA_FILTER_LZMA2, .options = &lzma_options},
        { .id = LZMA_VLI_UNKNOWN, .options = nullptr }
    };

    lzma_filter filters [] = {
        { .id = LZMA_FILTER_LZMA2, .options = &lzma_options},
        { .id = LZMA_VLI_UNKNOWN, .options = nullptr }
    };

    lzma_ret result = lzma_stream_encoder(&context, parameters.m_x86FilterActive ? filters_x86 : filters, LZMA_CHECK_CRC64);

    switch (result)
    {
        case LZMA_OK:
            return;
        case LZMA_MEM_ERROR:
        case LZMA_MEMLIMIT_ERROR:
            throw PartsException("Memory allocation error");
        case LZMA_NO_CHECK:
        case LZMA_UNSUPPORTED_CHECK:
        case LZMA_GET_CHECK:
        case LZMA_FORMAT_ERROR:
        case LZMA_OPTIONS_ERROR:
        case LZMA_DATA_ERROR:
        case LZMA_BUF_ERROR:
        case LZMA_PROG_ERROR:
        case LZMA_STREAM_END:
            throw PartsException("Compressior initialization error");
    }
}

}

//==========================================================================================================================================
LzmaCompressor::LzmaCompressor(const LzmaCompressorParameters& parameters) : m_parameters(parameters)
{
}

//==========================================================================================================================================
LzmaCompressor::~LzmaCompressor()
{
}

//==========================================================================================================================================
size_t LzmaCompressor::compressFile(const std::filesystem::path& path, ContentWriteBackend& backend)
{
    lzma_stream context;
    setupXZLib(context, m_parameters);
    auto guard = SimpleGuard<std::function<void()>>([&](){lzma_end(&context); });
    size_t compressed_size = 0;

    std::ifstream input_file(path.string(), std::ios::binary | std::ios::in);
    if (!input_file)
        throw PartsException("Cannot find file to compress: " + path.string());

    std::vector<uint8_t> output(MB,0);
    std::vector<uint8_t> input(MB,0);

    context.next_in = nullptr;
    context.avail_in = 0;
    context.next_out = &output[0];
    context.avail_out = MB;

    lzma_action action = LZMA_RUN;

    for(;;) {
        if (context.avail_in == 0 && !input_file.eof()) {
            input_file.read(reinterpret_cast<char*>(&input[0]), MB);

            if (input_file.bad())
                throw PartsException("Error during reading file: " + path.string());

            context.next_in = &input[0];
            context.avail_in = input_file.gcount();

            if (input_file.eof())
                action = LZMA_FINISH;
        }

        lzma_ret result = lzma_code(&context, action);

        if (context.avail_out == 0 || result == LZMA_STREAM_END) {
            size_t real_size = MB - context.avail_out;
            output.resize(real_size);
            compressed_size += real_size;
            backend.append(output);
            output.resize(MB);

            context.next_out = &output[0];
            context.avail_out = MB;
        }

        if (result == LZMA_OK)
            continue;
        else if (result == LZMA_STREAM_END)
            break;
        else
            throw PartsException("Unknown compression error during file: " + path.string());
    }

    return compressed_size;
}

//==========================================================================================================================================
size_t LzmaCompressor::compressBuffer(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& backend)
{
    lzma_stream context;
    setupXZLib(context, m_parameters);
    auto guard = SimpleGuard<std::function<void()>>([&](){lzma_end(&context); });
    size_t compressed_size = 0;

    std::vector<uint8_t> output(MB,0);

    context.next_in = &buffer[0];
    context.avail_in = buffer.size();
    context.next_out = &output[0];
    context.avail_out = MB;

    for(;;) {
        lzma_ret result = lzma_code(&context, LZMA_FINISH);
        if (context.avail_out == 0 || result == LZMA_STREAM_END) {
            size_t real_size = MB - context.avail_out;
            output.resize(real_size);
            compressed_size += real_size;
            backend.insert(backend.end(), output.begin(), output.end());
            output.resize(MB);
            context.next_out = &output[0];
            context.avail_out = MB;
        }

        if (result == LZMA_OK)
            continue;
        else if (result == LZMA_STREAM_END)
            break;
        else
            throw PartsException("Unknown compression error");
    }

    return compressed_size;
}
