#include <boost/test/auto_unit_test.hpp>
#include <filesystem>

#include "../parts_definitions.h"

#include "../filewritebackend.h"

using namespace parts;

//==========================================================================================================================================
struct FileWriteBackendFixture : FileWriteBackend {
    FileWriteBackendFixture() : FileWriteBackend("/tmp/try")
    {}

    ~FileWriteBackendFixture() {
        //std::filesystem::remove("/tmp/try");
    }
};

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(we_can_open_files) {
    if (std::filesystem::exists("/tmp/try1"))
        std::filesystem::remove("/tmp/try1");
    FileWriteBackend backend("/tmp/try1");

    BOOST_REQUIRE(std::filesystem::exists("/tmp/try1"));
}

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(we_can_write_data_with_correct_byteorder, FileWriteBackendFixture) {

    uint8_t data1  = 0x01;
    uint16_t data2 = 0x0201;
    uint32_t data3 = 0x04030201;
    uint64_t data4 = 0x0807060504030201L;

    std::vector<uint8_t> data5{0xf0, 0xf1, 0xf2, 0xf3, 0xf4};

    BOOST_CHECK_EQUAL(getPosition(), 0);

    append(data1);
    BOOST_CHECK_EQUAL(getPosition(), 1);

    append(data2);
    BOOST_CHECK_EQUAL(getPosition(), 3);

    append(data3);
    BOOST_CHECK_EQUAL(getPosition(), 7);

    append(data4);
    BOOST_CHECK_EQUAL(getPosition(), 15);

    append(data5);
    BOOST_CHECK_EQUAL(getPosition(), 20);

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

    BOOST_CHECK_EQUAL_COLLECTIONS(reference.begin(), reference.end(), file_data.begin(), file_data.end());
}


//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(we_throw_exception_if_file_is_closed, FileWriteBackendFixture) {
    m_file.close();

    BOOST_REQUIRE_EXCEPTION(checkFile(),
                            PartsException,
                            [](const PartsException& e){return e.what() == std::string("Internal Error: file has been closed!");});

    BOOST_REQUIRE_EXCEPTION(append(static_cast<uint8_t>(0)),
                            PartsException,
                            [](const PartsException& e){return e.what() == std::string("Internal Error: file has been closed!");});

    BOOST_REQUIRE_EXCEPTION(append(static_cast<uint16_t>(0)),
                            PartsException,
                            [](const PartsException& e){return e.what() == std::string("Internal Error: file has been closed!");});

    BOOST_REQUIRE_EXCEPTION(append(static_cast<uint32_t>(0)),
                            PartsException,
                            [](const PartsException& e){return e.what() == std::string("Internal Error: file has been closed!");});

    BOOST_REQUIRE_EXCEPTION(append(static_cast<uint64_t>(0)),
                            PartsException,
                            [](const PartsException& e){return e.what() == std::string("Internal Error: file has been closed!");});

    BOOST_REQUIRE_EXCEPTION(append(std::vector<uint8_t>(10,0)),
                            PartsException,
                            [](const PartsException& e){return e.what() == std::string("Internal Error: file has been closed!");});

    BOOST_REQUIRE_EXCEPTION(getPosition(),
                            PartsException,
                            [](const PartsException& e){return e.what() == std::string("Internal Error: file has been closed!");});

}


//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(we_can_read_back_the_written_content, FileWriteBackendFixture) {
    append(static_cast<uint32_t>(0xaabbccdd));
    append(static_cast<uint32_t>(0x00112233));

    BOOST_REQUIRE_NO_THROW(resetToRead());
    std::vector<uint8_t> data;
    BOOST_REQUIRE_NO_THROW(read(data));

    std::vector<uint8_t> reference = {0xaa, 0xbb, 0xcc, 0xdd, 0x00, 0x11, 0x22, 0x33};

    BOOST_CHECK_EQUAL_COLLECTIONS(reference.begin(), reference.end(), data.begin(), data.end());
}


//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(we_can_concatenate_files, FileWriteBackendFixture) {
    append(static_cast<uint32_t>(0xaabbccdd));

    FileWriteBackend right("/tmp/try2");
    right.append(static_cast<uint32_t>(0x00112233));

    BOOST_REQUIRE_NO_THROW(concatenate(std::move(right)));

    m_file.flush();
    m_file.close();

    std::vector<uint8_t> reference = {0xaa, 0xbb, 0xcc, 0xdd, 0x00, 0x11, 0x22, 0x33};

    std::ifstream test_file("/tmp/try", std::ios::binary);
    std::vector<uint8_t> file_data((std::istreambuf_iterator<char>(test_file)),
                                   std::istreambuf_iterator<char>());

    BOOST_CHECK_EQUAL_COLLECTIONS(reference.begin(), reference.end(), file_data.begin(), file_data.end());
}
