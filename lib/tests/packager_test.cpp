#include <boost/test/auto_unit_test.hpp>

#include "../packager.h"

#include <boost/endian/conversion.hpp>

using namespace parts;



//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_byte) {
    InputBuffer input = {1, 2, 3};
    uint8_t result = 0;

    Packager::pop_front(input, result);

    BOOST_REQUIRE_EQUAL(input.size(), 2);
    BOOST_REQUIRE_EQUAL(result, 1);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_short) {
    InputBuffer input = {0x02, 0x01};
    uint16_t result = 0;

    Packager::pop_front(input, result);

    BOOST_REQUIRE_EQUAL(input.size(), 0u);
    BOOST_REQUIRE_EQUAL(result, 0x0201);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_int) {
    InputBuffer input = {0x04, 0x03, 0x02, 0x01};
    uint32_t result = 0;

    Packager::pop_front(input, result);

    BOOST_REQUIRE_EQUAL(input.size(), 0u);
    BOOST_REQUIRE_EQUAL(result, 0x04030201);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_long) {
    InputBuffer input = { 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
    uint64_t result = 0;

    Packager::pop_front(input, result);

    BOOST_REQUIRE_EQUAL(input.size(), 0u);
    BOOST_REQUIRE_EQUAL(result, 0x0807060504030201);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_vector) {
    InputBuffer input = {0, 1, 2, 3, 4};
    std::vector<uint8_t> result(5, 0);

    Packager::pop_front(input, result);

    BOOST_REQUIRE_EQUAL(input.size(), 0);
    BOOST_REQUIRE_EQUAL(result[0], 0);
    BOOST_REQUIRE_EQUAL(result[1], 1);
    BOOST_REQUIRE_EQUAL(result[2], 2);
    BOOST_REQUIRE_EQUAL(result[3], 3);
    BOOST_REQUIRE_EQUAL(result[4], 4);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_string_length_1) {
    InputBuffer input = {4, 't', 'e', 'x', 't'};
    std::string txt;

    Packager::pop_front<uint8_t>(input, txt);

    BOOST_REQUIRE_EQUAL(input.size(), 0u);
    BOOST_REQUIRE_EQUAL(txt, "text");
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_string_length_2) {
    InputBuffer input = {0, 4, 't', 'e', 'x', 't'};
    std::string txt;

    Packager::pop_front<uint16_t>(input, txt);

    BOOST_REQUIRE_EQUAL(input.size(), 0u);
    BOOST_REQUIRE_EQUAL(txt, "text");
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_string_length_4) {
    InputBuffer input = {0, 0, 0, 4, 't', 'e', 'x', 't'};
    std::string txt;

    Packager::pop_front<uint32_t>(input, txt);

    BOOST_REQUIRE_EQUAL(input.size(), 0u);
    BOOST_REQUIRE_EQUAL(txt, "text");
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_path_with_two_length_names) {
    InputBuffer input = {0, 14, '/', 'u', 's', 'r', '/', 's', 'h', 'a', 'r', 'e', '/', 'l', 'i', 'b'};
    std::filesystem::path path;

    Packager::pop_front(input, path);

    BOOST_REQUIRE_EQUAL(input.size(), 0u);
    BOOST_REQUIRE_EQUAL(path, "/usr/share/lib");
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throw_exception_if_no_enough_data_for_scalars) {
    InputBuffer input = {0};
    uint16_t result;

    BOOST_REQUIRE_THROW(Packager::pop_front(input, result), PartsException);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throw_exception_if_no_enough_data_for_vector) {
    InputBuffer input = {0};
    std::vector<uint8_t> result(2,0);

    BOOST_REQUIRE_THROW(Packager::pop_front(input, result), PartsException);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throw_exception_if_no_enough_data_for_string_size) {
    InputBuffer input = {0};
    std::string result;

    BOOST_REQUIRE_THROW(Packager::pop_front<uint16_t>(input, result), PartsException);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throw_exception_if_no_enough_data_for_string_data) {
    InputBuffer input = {0, 4, 't'};
    std::string result;

    BOOST_REQUIRE_THROW(Packager::pop_front<uint16_t>(input, result), PartsException);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_byte) {
    std::vector<uint8_t> result;
    uint8_t byte = 4;

    Packager::append(result, byte);

    BOOST_REQUIRE_EQUAL(result.size(), 1u);
    BOOST_REQUIRE_EQUAL(result[0], 4);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_short) {
    std::vector<uint8_t> result;
    uint16_t data = 0x0201;

    Packager::append(result, data);

    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 0x02);
    BOOST_REQUIRE_EQUAL(result[1], 0x01);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_int) {
    std::vector<uint8_t> result;
    uint32_t data = 0x04030201;

    Packager::append(result, data);

    BOOST_REQUIRE_EQUAL(result.size(), 4u);

    BOOST_REQUIRE_EQUAL(result[0], 0x04);
    BOOST_REQUIRE_EQUAL(result[1], 0x03);
    BOOST_REQUIRE_EQUAL(result[2], 0x02);
    BOOST_REQUIRE_EQUAL(result[3], 0x01);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_long) {
    std::vector<uint8_t> result;
    uint64_t data = 0x0807060504030201;

    Packager::append(result, data);

    BOOST_REQUIRE_EQUAL(result.size(), 8u);

    BOOST_REQUIRE_EQUAL(result[0], 0x08);
    BOOST_REQUIRE_EQUAL(result[1], 0x07);
    BOOST_REQUIRE_EQUAL(result[2], 0x06);
    BOOST_REQUIRE_EQUAL(result[3], 0x05);
    BOOST_REQUIRE_EQUAL(result[4], 0x04);
    BOOST_REQUIRE_EQUAL(result[5], 0x03);
    BOOST_REQUIRE_EQUAL(result[6], 0x02);
    BOOST_REQUIRE_EQUAL(result[7], 0x01);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_vector) {
    std::vector<uint8_t> result;
    std::vector<uint8_t> data = {0, 1, 2, 3, 4};

    Packager::append(result, data);

    BOOST_REQUIRE_EQUAL(result.size(), 5);
    BOOST_REQUIRE_EQUAL(result[0], 0);
    BOOST_REQUIRE_EQUAL(result[1], 1);
    BOOST_REQUIRE_EQUAL(result[2], 2);
    BOOST_REQUIRE_EQUAL(result[3], 3);
    BOOST_REQUIRE_EQUAL(result[4], 4);
}

namespace
{
void check_text(const std::vector<uint8_t>& result, size_t offset) {
    BOOST_REQUIRE_EQUAL(result[0 + offset], 't');
    BOOST_REQUIRE_EQUAL(result[1 + offset], 'e');
    BOOST_REQUIRE_EQUAL(result[2 + offset], 'x');
    BOOST_REQUIRE_EQUAL(result[3 + offset], 't');
}
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_string_length_1) {
    std::vector<uint8_t> result;
    std::string txt("text");

    Packager::append<uint8_t>(result, txt);

    BOOST_REQUIRE_EQUAL(result.size(), 5u);
    BOOST_REQUIRE_EQUAL(result[0], 4u);
    check_text(result, 1);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_string_length_2) {
    std::vector<uint8_t> result;
    std::string txt("text");

    Packager::append<uint16_t>(result, txt);

    BOOST_REQUIRE_EQUAL(result.size(), 6u);
    BOOST_REQUIRE_EQUAL(result[0], 0u);
    BOOST_REQUIRE_EQUAL(result[1], 4u);
    check_text(result, 2);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_string_length_4) {
    std::vector<uint8_t> result;
    std::string txt("text");

    Packager::append<uint32_t>(result, txt);

    BOOST_REQUIRE_EQUAL(result.size(), 8u);
    BOOST_REQUIRE_EQUAL(result[0], 0u);
    BOOST_REQUIRE_EQUAL(result[1], 0u);
    BOOST_REQUIRE_EQUAL(result[2], 0u);
    BOOST_REQUIRE_EQUAL(result[3], 4u);
    check_text(result, 4);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(string_pack_throw_exception_if_length_is_too_short) {
    std::vector<uint8_t> result;
    std::string txt(300,'a');

    BOOST_REQUIRE_THROW(Packager::append<uint8_t>(result, txt), PartsException);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_path_with_two_length_names) {
    std::vector<uint8_t> result;
    std::filesystem::path path("/usr/share/lib");

    Packager::append(result, path);

    BOOST_REQUIRE_EQUAL(result.size(), 14u + 2u);
    BOOST_REQUIRE_EQUAL(result[2], '/');
    BOOST_REQUIRE_EQUAL(result[6], '/');
    BOOST_REQUIRE_EQUAL(result[7], 's');
    BOOST_REQUIRE_EQUAL(result[15], 'b');
}
