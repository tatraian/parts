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

    Mock<ContentWriteBackend> mock;
    When(OverloadedMethod(mock, append, void(const std::vector<uint8_t>&))).AlwaysReturn();

    ContentWriteBackend& backend = mock.get();

    compressor.compressBuffer(data, backend);

    Verify(OverloadedMethod(mock, append, void(const std::vector<uint8_t>&)).Using(data)).Once();
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throws_if_file_not_exists) {
    PlainCompressor compressor;
    boost::filesystem::path path ("not_existing/file");

    Mock<ContentWriteBackend> mock;

    BOOST_REQUIRE_EXCEPTION(compressor.compressFile(path, mock.get()),
                            PartsException,
                            [](const std::exception& e){
                                return e.what() == std::string("Cannot open file for reading: not_existing/file");
                            });

}
