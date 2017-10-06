#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include <fstream>

#include "../lzmacompressor.h"
#include "../parts_definitions.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;

namespace
{

void check_data(const std::vector<uint8_t>& input,
                const std::vector<uint8_t>& compressed)
{
    lzma_stream lzma_context = LZMA_STREAM_INIT;
    lzma_ret result = lzma_stream_decoder(&lzma_context, UINT64_MAX, LZMA_CONCATENATED);

    std::vector<uint8_t> uncompressed(MB, 0);
    lzma_context.next_in = &compressed[0];
    lzma_context.avail_in = compressed.size();
    lzma_context.next_out = &uncompressed[0];
    lzma_context.avail_out = uncompressed.size();

    result = lzma_code(&lzma_context, LZMA_FINISH);


    BOOST_REQUIRE_EQUAL(result, LZMA_STREAM_END);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(input.begin(), input.end(), uncompressed.begin(), uncompressed.end());

    lzma_end(&lzma_context);
}

}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(lzma_can_compress_buffer) {
    std::vector<uint8_t> input(MB, 0);
    for(size_t tmp = 0; tmp != input.size(); ++tmp)
        input[tmp] = tmp;

    LzmaCompressorParameters parameters;
    LzmaCompressor compressor(parameters);

    std::vector<uint8_t> compressed;

    Mock<ContentWriteBackend> mock;
    When(OverloadedMethod(mock, append, void(const std::vector<uint8_t>&))).AlwaysDo([&](const std::vector<uint8_t>& a){compressed = a;});

    compressor.compressBuffer(input, mock.get());

    check_data(input, compressed);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(lzma_can_compress_file) {
    boost::filesystem::path path("/tmp/parts_unit_test_never_write_file");

    std::vector<uint8_t> input(MB, 0);
    for(size_t tmp = 0; tmp != input.size(); ++tmp)
        input[tmp] = tmp;

    std::ofstream file(path.string(), std::ios::binary | std::ios::ate);
    file.write(reinterpret_cast<char*>(&input[0]), input.size());
    file.close();

    std::vector<uint8_t> compressed;

    Mock<ContentWriteBackend> mock;
    When(OverloadedMethod(mock, append, void(const std::vector<uint8_t>&))).AlwaysDo([&](const std::vector<uint8_t>& a){compressed = a;});

    LzmaCompressorParameters parameters;
    LzmaCompressor compressor(parameters);

    compressor.compressFile(path, mock.get());

    check_data(input, compressed);
}
