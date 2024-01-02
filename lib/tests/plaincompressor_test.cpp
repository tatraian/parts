#include <gmock/gmock.h>
#include <fakeit.hpp>

#include "../plaincompressor.h"
#include "../parts_definitions.h"

using namespace parts;
using namespace fakeit;

//==========================================================================================================================================
TEST(plaincompressor, can_compress_buffer) {
    PlainCompressor compressor;
    std::vector<uint8_t> data = {1,2,3,4};

    std::vector<uint8_t> result;

    compressor.compressBuffer(data, result);

    ASSERT_EQ(data.size(), result.size());
    ASSERT_THAT(data, result);
}

//==========================================================================================================================================
TEST(plaincompressor, throws_if_file_not_exists) {
    PlainCompressor compressor;
    std::filesystem::path path ("not_existing/file");

    Mock<ContentWriteBackend> mock;

    ASSERT_THROW(compressor.compressFile(path, mock.get()), PartsException);

}
