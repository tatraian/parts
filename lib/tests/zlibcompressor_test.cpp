#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include <fstream>
#include <zlib.h>

#include "../zlibcompressor.h"
#include "../parts_definitions.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;

namespace
{

//==========================================================================================================================================
void check_data(const std::vector<uint8_t>& input,
                const std::vector<uint8_t>& compressed)
{
    z_stream context;
    context.zalloc = Z_NULL;
    context.zfree = Z_NULL;
    context.opaque = Z_NULL;
    int result = inflateInit(&context);

    std::vector<uint8_t> uncompressed(MB, 0);
    context.next_in = const_cast<uint8_t *>(&compressed[0]);
    context.avail_in = compressed.size();
    context.next_out = &uncompressed[0];
    context.avail_out = uncompressed.size();

    result = inflate(&context, Z_FINISH);

    BOOST_REQUIRE_EQUAL(result, Z_STREAM_END);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(input.begin(), input.end(), uncompressed.begin(), uncompressed.end());

    inflateEnd(&context);
}

}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(zlib_can_compress_buffer) {
    std::vector<uint8_t> input;
    for(size_t tmp = 0; tmp != MB; ++tmp)
        input.push_back(tmp);

    ZLibCompressor compressor;
    std::vector<uint8_t> compressed;
    compressor.compressBuffer(input, compressed);
    check_data(input, compressed);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(zlib_can_compress_file) {
    std::filesystem::path path("/tmp/parts_unit_test_never_write_file");

    std::vector<uint8_t> input(MB, 0);
    for(size_t tmp = 0; tmp != input.size(); ++tmp)
        input[tmp] = tmp;

    std::ofstream file(path.string(), std::ios::binary | std::ios::ate);
    file.write(reinterpret_cast<char*>(&input[0]), input.size());
    file.close();

    std::vector<uint8_t> compressed;

    Mock<ContentWriteBackend> mock;
    When(OverloadedMethod(mock, append, void(const std::vector<uint8_t>&))).AlwaysDo([&](const std::vector<uint8_t>& a){compressed = a;});

    ZLibCompressor compressor;

    compressor.compressFile(path, mock.get());

    std::vector<uint8_t> converted_input(MB);
    converted_input.clear();
    converted_input.insert(converted_input.end(), input.begin(), input.end());

    check_data(converted_input, compressed);
}
