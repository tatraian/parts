#include <gmock/gmock.h>

#include "../filereadbackend.h"
#include "../parts_definitions.h"

#include <fstream>
#include <filesystem>


using namespace parts;

namespace {
void create_file(const std::string& name, const std::vector<uint8_t>& data) {
    std::ofstream file(name, std::ios::binary | std::ios::ate);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}
const std::string read_test = "/tmp/read_test";
}


//==========================================================================================================================================
TEST(file_read, can_open_file) {
    std::ofstream test_file("/tmp/opentest");
    test_file.close();

    std::unique_ptr<FileReadBackend> backend;
    backend.reset(new FileReadBackend (std::filesystem::path("/tmp/opentest")));

    ASSERT_TRUE(*backend);
    ASSERT_EQ(backend->source(), "/tmp/opentest");
}

//==========================================================================================================================================
TEST(file_read, throws_if_file_not_exist) {
    std::error_code err;
    std::filesystem::remove("/tmp/opentest", err);

    FileReadBackend backend("/tmp/opentest");
    ASSERT_TRUE(!backend);
}

//==========================================================================================================================================
TEST(file_read, file_backend_can_read_to_vector) {
    create_file(read_test, {0,0,1,0,0});

    FileReadBackend backend(read_test);
    std::vector<uint8_t> result(3, 0);

    backend.read(result);
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result[0], 0);
    ASSERT_EQ(result[1], 0);
    ASSERT_EQ(result[2], 1);
}

//==========================================================================================================================================
TEST(file_read, file_backend_can_read_to_array) {
    create_file(read_test, {0,0,1,0,0});

    FileReadBackend backend(read_test);
    uint8_t result[3] = {0, 0, 0};

    backend.read(result, 3);
    ASSERT_EQ(result[0], 0);
    ASSERT_EQ(result[1], 0);
    ASSERT_EQ(result[2], 1);
}

//==========================================================================================================================================
TEST(file_read, file_backend_can_seek_in_file) {
    create_file(read_test, {0,1,2,3});

    FileReadBackend backend(read_test);
    backend.seek(1);

    std::vector<uint8_t> result(2, 0);
    backend.read(result);

    ASSERT_EQ(result[0], 0x01);
    ASSERT_EQ(result[1], 0x02);
}


//==========================================================================================================================================
TEST(file_read, file_backend_throws_if_there_is_no_enough_data_scalar) {
    create_file(read_test, {0,1,2,3});

    FileReadBackend backend(read_test);

    std::vector<uint8_t> result(6, 0);
    ASSERT_THROW(backend.read(result), PartsException);
}

//==========================================================================================================================================
TEST(file_read, file_backend_throws_if_there_is_no_enough_data_array) {
    create_file(read_test, {0,1,2,3});

    FileReadBackend backend(read_test);

    std::vector<uint8_t> result(6, 0);
    ASSERT_THROW(backend.read(result), PartsException);
}

//==========================================================================================================================================
TEST(file_read, file_backend_seek_throws_error_in_case_of_out_of_range) {
    create_file(read_test, {0,1,2,3});

    FileReadBackend backend(read_test);
    ASSERT_THROW(backend.seek(4), PartsException);
}
