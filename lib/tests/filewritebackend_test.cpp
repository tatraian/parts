#include <gmock/gmock.h>
#include <filesystem>

#include "../parts_definitions.h"

#include "../filewritebackend.h"

using namespace parts;

//==========================================================================================================================================
struct FileWriteBackendFixture : FileWriteBackend, ::testing::Test {
    FileWriteBackendFixture() : FileWriteBackend("/tmp/try")
    {}

    ~FileWriteBackendFixture() {
        //std::filesystem::remove("/tmp/try");
    }
};

//==========================================================================================================================================
TEST(file_writer_backend, we_can_open_files) {
    if (std::filesystem::exists("/tmp/try1"))
        std::filesystem::remove("/tmp/try1");
    FileWriteBackend backend("/tmp/try1");

    ASSERT_TRUE(std::filesystem::exists("/tmp/try1"));
}

//==========================================================================================================================================
TEST_F(FileWriteBackendFixture, we_can_write_data_with_correct_byteorder) {

    uint8_t data1  = 0x01;
    uint16_t data2 = 0x0201;
    uint32_t data3 = 0x04030201;
    uint64_t data4 = 0x0807060504030201L;

    std::vector<uint8_t> data5{0xf0, 0xf1, 0xf2, 0xf3, 0xf4};

    ASSERT_EQ(getPosition(), 0);

    append(data1);
    ASSERT_EQ(getPosition(), 1);

    append(data2);
    ASSERT_EQ(getPosition(), 3);

    append(data3);
    ASSERT_EQ(getPosition(), 7);

    append(data4);
    ASSERT_EQ(getPosition(), 15);

    append(data5);
    ASSERT_EQ(getPosition(), 20);

    m_file.flush();
    m_file.close();

    std::vector<uint8_t> reference = {0x01,
                                      0x02, 0x01,
                                      0x04, 0x03, 0x02, 0x01,
                                      0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
                                      0xf0, 0xf1, 0xf2, 0xf3, 0xf4};

    std::ifstream test_file("/tmp/try", std::ios::binary);
    std::vector<uint8_t> file_data((std::istreambuf_iterator<char>(test_file)),
                                   std::istreambuf_iterator<char>());

    ASSERT_THAT(reference, file_data);
}


//==========================================================================================================================================
TEST_F(FileWriteBackendFixture, we_throw_exception_if_file_is_closed) {
    m_file.close();

    ASSERT_THROW(checkFile(), PartsException);

    ASSERT_THROW(append(static_cast<uint8_t>(0)), PartsException);
    ASSERT_THROW(append(static_cast<uint16_t>(0)), PartsException);
    ASSERT_THROW(append(static_cast<uint32_t>(0)), PartsException);
    ASSERT_THROW(append(static_cast<uint64_t>(0)), PartsException);
    ASSERT_THROW(append(std::vector<uint8_t>(10,0)), PartsException);
    ASSERT_THROW(getPosition(), PartsException);
}


//==========================================================================================================================================
TEST_F(FileWriteBackendFixture, we_can_read_back_the_written_content) {
    append(static_cast<uint32_t>(0xaabbccdd));
    append(static_cast<uint32_t>(0x00112233));

    ASSERT_NO_THROW(resetToRead());
    std::vector<uint8_t> data;
    ASSERT_NO_THROW(read(data));

    std::vector<uint8_t> reference = {0xaa, 0xbb, 0xcc, 0xdd, 0x00, 0x11, 0x22, 0x33};

    ASSERT_THAT(reference, data);
}


//==========================================================================================================================================
TEST_F(FileWriteBackendFixture, we_can_concatenate_files) {
    append(static_cast<uint32_t>(0xaabbccdd));

    FileWriteBackend right("/tmp/try2");
    right.append(static_cast<uint32_t>(0x00112233));

    ASSERT_NO_THROW(concatenate(std::move(right)));

    m_file.flush();
    m_file.close();

    std::vector<uint8_t> reference = {0xaa, 0xbb, 0xcc, 0xdd, 0x00, 0x11, 0x22, 0x33};

    std::ifstream test_file("/tmp/try", std::ios::binary);
    std::vector<uint8_t> file_data((std::istreambuf_iterator<char>(test_file)),
                                   std::istreambuf_iterator<char>());

    ASSERT_THAT(reference, file_data);
}
