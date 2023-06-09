#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include "../plaincompressor.h"
#include "../parts_definitions.h"

using namespace parts;
using namespace fakeit;

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_compress_buffer) {
    PlainCompressor compressor;
    std::vector<uint8_t> data = {1,2,3,4};

    std::vector<uint8_t> result;

    compressor.compressBuffer(data, result);

    BOOST_REQUIRE_EQUAL(data.size(), result.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(data.begin(), data.end(), result.begin(), result.end());
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throws_if_file_not_exists) {
    PlainCompressor compressor;
    std::filesystem::path path ("not_existing/file");

    Mock<ContentWriteBackend> mock;

    BOOST_REQUIRE_EXCEPTION(compressor.compressFile(path, mock.get()),
                            PartsException,
                            [](const std::exception& e){
                                return e.what() == std::string("Cannot open file for reading: not_existing/file");
                            });

}
