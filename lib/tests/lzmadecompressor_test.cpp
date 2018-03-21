#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include <fstream>
#include <lzma.h>

#include "../lzmadecompressor.h"
#include "../parts_definitions.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;

namespace
{

//==========================================================================================================================================
std::vector<uint8_t> create_data(const std::vector<uint8_t>& input)
{
    lzma_stream lzma_context = LZMA_STREAM_INIT;
    lzma_ret result = lzma_easy_encoder(&lzma_context, 9, LZMA_CHECK_CRC64);

    std::vector<uint8_t> compressed(MB, 0);
    lzma_context.next_in = &input[0];
    lzma_context.avail_in = input.size();
    lzma_context.next_out = &compressed[0];
    lzma_context.avail_out = compressed.size();

    result = lzma_code(&lzma_context, LZMA_FINISH);

    compressed.resize(compressed.size() - lzma_context.avail_out);
    lzma_end(&lzma_context);

    return compressed;
}
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_decompress_lzma_buffer_smaller_than_mb) {

    std::vector<uint8_t> input = {1,2,3,4,5,6,7,8,9,0};

    LzmaDecompressor decomp;
    InputBuffer result = decomp.extractBuffer(create_data(input), input.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(input.begin(), input.end(), result.begin(), result.end());
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_decompress_lzma_buffer_greater_than_mb) {

    std::vector<uint8_t> input(MB + 100);
    for (size_t tmp = 0; tmp != input.size(); ++tmp) {
        input[tmp] = tmp;
    }

    LzmaDecompressor decomp;
    InputBuffer result = decomp.extractBuffer(create_data(input), input.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(input.begin(), input.end(), result.begin(), result.end());
}

// TODO after filereadbackend implementation file extraction test here.
