#include <boost/test/auto_unit_test.hpp>

#include "../filereadbackend.h"
#include "../parts_definitions.h"

#include <fstream>
#include <boost/filesystem.hpp>


using namespace parts;

namespace {
void create_file(const std::string& name, const std::vector<uint8_t>& data) {
    std::ofstream file(name, std::ios::binary | std::ios::ate);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}
const std::string read_test = "/tmp/read_test";
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_open_file) {
    std::ofstream test_file("/tmp/opentest");
    test_file.close();

    std::unique_ptr<FileReadBackend> backend;
    backend.reset(new FileReadBackend (boost::filesystem::path("/tmp/opentest")));

    BOOST_CHECK(*backend);
    BOOST_CHECK_EQUAL(backend->source(), "/tmp/opentest");
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throws_if_file_not_exist) {
    boost::system::error_code err;
    boost::filesystem::remove("/tmp/opentest", err);

    FileReadBackend backend("/tmp/opentest");
    BOOST_CHECK(!backend);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(file_backend_can_read_to_vector) {
    create_file(read_test, {0,0,1,0,0});

    FileReadBackend backend(read_test);
    std::vector<uint8_t> result(3, 0);

    backend.read(result);
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], 0);
    BOOST_CHECK_EQUAL(result[1], 0);
    BOOST_CHECK_EQUAL(result[2], 1);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(file_backend_can_read_to_array) {
    create_file(read_test, {0,0,1,0,0});

    FileReadBackend backend(read_test);
    uint8_t result[3] = {0, 0, 0};

    backend.read(result, 3);
    BOOST_CHECK_EQUAL(result[0], 0);
    BOOST_CHECK_EQUAL(result[1], 0);
    BOOST_CHECK_EQUAL(result[2], 1);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(file_backend_can_seek_in_file) {
    create_file(read_test, {0,1,2,3});

    FileReadBackend backend(read_test);
    backend.seek(1);

    std::vector<uint8_t> result(2, 0);
    backend.read(result);

    BOOST_CHECK_EQUAL(result[0], 0x01);
    BOOST_CHECK_EQUAL(result[1], 0x02);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(file_backend_throws_if_there_is_no_enough_data_scalar) {
    create_file(read_test, {0,1,2,3});

    FileReadBackend backend(read_test);

    std::vector<uint8_t> result(6, 0);
    BOOST_CHECK_THROW(backend.read(result), PartsException);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(file_backend_throws_if_there_is_no_enough_data_array) {
    create_file(read_test, {0,1,2,3});

    FileReadBackend backend(read_test);

    std::vector<uint8_t> result(6, 0);
    BOOST_CHECK_THROW(backend.read(result), PartsException);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(file_backend_seek_throws_error_in_case_of_out_of_range) {
    create_file(read_test, {0,1,2,3});

    FileReadBackend backend(read_test);
    BOOST_CHECK_THROW(backend.seek(4), PartsException);

}
