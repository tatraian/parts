#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include <fstream>
#include <zlib.h>

#include "../zlibdecompressor.h"
#include "../parts_definitions.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;

namespace
{

//==========================================================================================================================================
std::vector<uint8_t> create_data(const std::vector<uint8_t>& input)
{
    z_stream context;
    context.zalloc = Z_NULL;
    context.zfree = Z_NULL;
    context.opaque = Z_NULL;

    int result = deflateInit(&context, Z_BEST_COMPRESSION);

    std::vector<uint8_t> compressed(MB, 0);
    context.next_in = const_cast<uint8_t *>(&input[0]);
    context.avail_in = input.size();
    context.next_out = &compressed[0];
    context.avail_out = compressed.size();

    result = deflate(&context, Z_FINISH);
    compressed.resize(compressed.size() - context.avail_out);
    deflateEnd(&context);

    return compressed;
}
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_decompress_zlib_buffer_smaller_than_mb) {

    std::vector<uint8_t> input = {1,2,3,4,5,6,7,8,9,0};

    ZlibDecompressor decomp;
    InputBuffer result = decomp.extractBuffer(create_data(input), input.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(input.begin(), input.end(), result.begin(), result.end());
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_decompress_zlib_buffer_greater_than_mb) {

    std::vector<uint8_t> input(MB + 100);
    for (size_t tmp = 0; tmp != input.size(); ++tmp) {
        input[tmp] = tmp;
    }

    ZlibDecompressor decomp;
    InputBuffer result = decomp.extractBuffer(create_data(input), input.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(input.begin(), input.end(), result.begin(), result.end());
}

// TODO after filereadbackend implementation file extraction test here.
