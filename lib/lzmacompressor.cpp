#include "lzmacompressor.h"

#include "parts_definitions.h"

using namespace parts;

template<class Functor>
struct SimpleGuard {
public:
    SimpleGuard(Functor fn) : m_destructor(fn) {}
    ~SimpleGuard() { m_destructor(); }

    Functor m_destructor;
};

//==========================================================================================================================================
LzmaCompressor::LzmaCompressor(const LzmaCompressorParameters& parameters) : m_parameters(parameters)
{
}

//==========================================================================================================================================
LzmaCompressor::~LzmaCompressor()
{
}

//==========================================================================================================================================
size_t LzmaCompressor::compressFile(const boost::filesystem::path& path, ContentWriteBackend& backend)
{
    lzma_stream context;
    setupXZLib(context);
    auto guard = SimpleGuard<std::function<void()>>([&](){lzma_end(&context); });
}

//==========================================================================================================================================
size_t LzmaCompressor::compressBuffer(const std::vector<uint8_t>& buffer, ContentWriteBackend& backend)
{
    lzma_stream context;
    setupXZLib(context);
    auto guard = SimpleGuard<std::function<void()>>([&](){lzma_end(&context); });



}

//==========================================================================================================================================
void LzmaCompressor::setupXZLib(lzma_stream& context)
{
    context = LZMA_STREAM_INIT;
    lzma_options_lzma lzma_options;
    if (lzma_lzma_preset(&lzma_options, m_parameters.m_compressionLevel))
        throw PartsException("Unsupported compression level: " + std::to_string(m_parameters.m_compressionLevel));

    lzma_filter filters_x86 [] = {
        { .id = LZMA_FILTER_X86, .options = nullptr },
        { .id = LZMA_FILTER_LZMA2, .options = &lzma_options},
        { .id = LZMA_VLI_UNKNOWN, .options = nullptr }
    };

    lzma_filter filters [] = {
        { .id = LZMA_FILTER_X86, .options = nullptr },
        { .id = LZMA_FILTER_LZMA2, .options = &lzma_options},
        { .id = LZMA_VLI_UNKNOWN, .options = nullptr }
    };

    lzma_mt config = {
        .flags = 0,
        .block_size = 0,
        .filters = m_parameters.m_x86FilterActive ? filters_x86 : filters,
        .check = LZMA_CHECK_CRC64,
        .threads = m_parameters.m_threads
    };
    lzma_ret result = lzma_stream_encoder_mt(&context, &config);

    if (result == LZMA_OK)
        return;

    switch (result) {
    case LZMA_MEM_ERROR:
        throw PartsException("Memory allocation error");
    default:
        throw PartsException("Compressior initialization error");
    }
}




