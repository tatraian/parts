#include <gmock/gmock.h>

#include "../packager.h"

using namespace parts;



//==========================================================================================================================================
TEST(packing, can_unpack_byte) {
    InputBuffer input = {1, 2, 3};
    uint8_t result = 0;

    Packager::pop_front(input, result);

    ASSERT_EQ(input.size(), 2);
    ASSERT_EQ(result, 1);
}


//==========================================================================================================================================
TEST(packing, can_unpack_short) {
    InputBuffer input = {0x02, 0x01};
    uint16_t result = 0;

    Packager::pop_front(input, result);

    ASSERT_EQ(input.size(), 0u);
    ASSERT_EQ(result, 0x0201);
}

//==========================================================================================================================================
TEST(packing, can_unpack_int) {
    InputBuffer input = {0x04, 0x03, 0x02, 0x01};
    uint32_t result = 0;

    Packager::pop_front(input, result);

    ASSERT_EQ(input.size(), 0u);
    ASSERT_EQ(result, 0x04030201);
}

//==========================================================================================================================================
TEST(packing, can_unpack_long) {
    InputBuffer input = { 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    uint64_t result = 0;

    Packager::pop_front(input, result);

    ASSERT_EQ(input.size(), 0u);
    ASSERT_EQ(result, 0x0807060504030201);
}

//==========================================================================================================================================
TEST(packing, can_unpack_vector) {
    InputBuffer input = {0, 1, 2, 3, 4};
    std::vector<uint8_t> result(5, 0);

    Packager::pop_front(input, result);

    ASSERT_EQ(input.size(), 0);
    ASSERT_EQ(result[0], 0);
    ASSERT_EQ(result[1], 1);
    ASSERT_EQ(result[2], 2);
    ASSERT_EQ(result[3], 3);
    ASSERT_EQ(result[4], 4);
}

//==========================================================================================================================================
TEST(packing, can_unpack_string_length_1) {
    InputBuffer input = {4, 't', 'e', 'x', 't'};
    std::string txt;

    Packager::pop_front<uint8_t>(input, txt);

    ASSERT_EQ(input.size(), 0u);
    ASSERT_EQ(txt, "text");
}

//==========================================================================================================================================
TEST(packing, can_unpack_string_length_2) {
    InputBuffer input = {0, 4, 't', 'e', 'x', 't'};
    std::string txt;

    Packager::pop_front<uint16_t>(input, txt);

    ASSERT_EQ(input.size(), 0u);
    ASSERT_EQ(txt, "text");
}

//==========================================================================================================================================
TEST(packing, can_unpack_string_length_4) {
    InputBuffer input = {0, 0, 0, 4, 't', 'e', 'x', 't'};
    std::string txt;

    Packager::pop_front<uint32_t>(input, txt);

    ASSERT_EQ(input.size(), 0u);
    ASSERT_EQ(txt, "text");
}

//==========================================================================================================================================
TEST(packing, can_unpack_path_with_two_length_names) {
    InputBuffer input = {0, 14, '/', 'u', 's', 'r', '/', 's', 'h', 'a', 'r', 'e', '/', 'l', 'i', 'b'};
    std::filesystem::path path;

    Packager::pop_front(input, path);

    ASSERT_EQ(input.size(), 0u);
    ASSERT_EQ(path, "/usr/share/lib");
}

//==========================================================================================================================================
TEST(packing, throw_exception_if_no_enough_data_for_scalars) {
    InputBuffer input = {0};
    uint16_t result;

    ASSERT_THROW(Packager::pop_front(input, result), PartsException);
}

//==========================================================================================================================================
TEST(packing, throw_exception_if_no_enough_data_for_vector) {
    InputBuffer input = {0};
    std::vector<uint8_t> result(2,0);

    ASSERT_THROW(Packager::pop_front(input, result), PartsException);
}

//==========================================================================================================================================
TEST(packing, throw_exception_if_no_enough_data_for_string_size) {
    InputBuffer input = {0};
    std::string result;

    ASSERT_THROW(Packager::pop_front<uint16_t>(input, result), PartsException);
}

//==========================================================================================================================================
TEST(packing, throw_exception_if_no_enough_data_for_string_data) {
    InputBuffer input = {0, 4, 't'};
    std::string result;

    ASSERT_THROW(Packager::pop_front<uint16_t>(input, result), PartsException);
}


//==========================================================================================================================================
TEST(packing, can_pack_byte) {
    std::vector<uint8_t> result;
    uint8_t byte = 4;

    Packager::append(result, byte);

    ASSERT_EQ(result.size(), 1u);
    ASSERT_EQ(result[0], 4);
}


//==========================================================================================================================================
TEST(packing, can_pack_short) {
    std::vector<uint8_t> result;
    uint16_t data = 0x0201;

    Packager::append(result, data);

    ASSERT_EQ(result.size(), 2u);
    ASSERT_EQ(result[0], 0x02);
    ASSERT_EQ(result[1], 0x01);
}


//==========================================================================================================================================
TEST(packing, can_pack_int) {
    std::vector<uint8_t> result;
    uint32_t data = 0x04030201;

    Packager::append(result, data);

    ASSERT_EQ(result.size(), 4u);

    ASSERT_EQ(result[0], 0x04);
    ASSERT_EQ(result[1], 0x03);
    ASSERT_EQ(result[2], 0x02);
    ASSERT_EQ(result[3], 0x01);
}

//==========================================================================================================================================
TEST(packing, can_pack_long) {
    std::vector<uint8_t> result;
    uint64_t data = 0x0807060504030201;

    Packager::append(result, data);

    ASSERT_EQ(result.size(), 8u);

    ASSERT_EQ(result[0], 0x08);
    ASSERT_EQ(result[1], 0x07);
    ASSERT_EQ(result[2], 0x06);
    ASSERT_EQ(result[3], 0x05);
    ASSERT_EQ(result[4], 0x04);
    ASSERT_EQ(result[5], 0x03);
    ASSERT_EQ(result[6], 0x02);
    ASSERT_EQ(result[7], 0x01);
}

//==========================================================================================================================================
TEST(packing, can_pack_vector) {
    std::vector<uint8_t> result;
    std::vector<uint8_t> data = {0, 1, 2, 3, 4};

    Packager::append(result, data);

    ASSERT_EQ(result.size(), 5);
    ASSERT_EQ(result[0], 0);
    ASSERT_EQ(result[1], 1);
    ASSERT_EQ(result[2], 2);
    ASSERT_EQ(result[3], 3);
    ASSERT_EQ(result[4], 4);
}

namespace
{
void check_text(const std::vector<uint8_t>& result, size_t offset) {
    ASSERT_EQ(result[0 + offset], 't');
    ASSERT_EQ(result[1 + offset], 'e');
    ASSERT_EQ(result[2 + offset], 'x');
    ASSERT_EQ(result[3 + offset], 't');
}
}

//==========================================================================================================================================
TEST(packing, can_pack_string_length_1) {
    std::vector<uint8_t> result;
    std::string txt("text");

    Packager::append<uint8_t>(result, txt);

    ASSERT_EQ(result.size(), 5u);
    ASSERT_EQ(result[0], 4u);
    check_text(result, 1);
}

//==========================================================================================================================================
TEST(packing, can_pack_string_length_2) {
    std::vector<uint8_t> result;
    std::string txt("text");

    Packager::append<uint16_t>(result, txt);

    ASSERT_EQ(result.size(), 6u);
    ASSERT_EQ(result[0], 0u);
    ASSERT_EQ(result[1], 4u);
    check_text(result, 2);
}

//==========================================================================================================================================
TEST(packing, can_pack_string_length_4) {
    std::vector<uint8_t> result;
    std::string txt("text");

    Packager::append<uint32_t>(result, txt);

    ASSERT_EQ(result.size(), 8u);
    ASSERT_EQ(result[0], 0u);
    ASSERT_EQ(result[1], 0u);
    ASSERT_EQ(result[2], 0u);
    ASSERT_EQ(result[3], 4u);
    check_text(result, 4);
}


//==========================================================================================================================================
TEST(packing, string_pack_throw_exception_if_length_is_too_short) {
    std::vector<uint8_t> result;
    std::string txt(300,'a');

    ASSERT_THROW(Packager::append<uint8_t>(result, txt), PartsException);
}

//==========================================================================================================================================
TEST(packing, can_pack_path_with_two_length_names) {
    std::vector<uint8_t> result;
    std::filesystem::path path("/usr/share/lib");

    Packager::append(result, path);

    ASSERT_EQ(result.size(), 14u + 2u);
    ASSERT_EQ(result[2], '/');
    ASSERT_EQ(result[6], '/');
    ASSERT_EQ(result[7], 's');
    ASSERT_EQ(result[15], 'b');
}
