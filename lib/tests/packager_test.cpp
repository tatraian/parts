#include <boost/test/auto_unit_test.hpp>

#include "../packager.h"

#include <boost/endian/conversion.hpp>

using namespace parts;

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_byte) {
    std::deque<uint8_t> result(1);
    result.clear();
    uint8_t byte = 4;

    Packager::append(result, byte);

    BOOST_REQUIRE_EQUAL(result.size(), 1u);
    BOOST_REQUIRE_EQUAL(result[0], 4);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_short) {
    std::deque<uint8_t> result(2);
    result.clear();
    uint16_t data = 0x0201;

    Packager::append(result, data);

    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 0x02);
    BOOST_REQUIRE_EQUAL(result[1], 0x01);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_int) {
    std::deque<uint8_t> result(4);
    result.clear();
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
    std::deque<uint8_t> result;
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

namespace
{
void check_text(const std::deque<uint8_t>& result, size_t offset) {
    BOOST_REQUIRE_EQUAL(result[0 + offset], 't');
    BOOST_REQUIRE_EQUAL(result[1 + offset], 'e');
    BOOST_REQUIRE_EQUAL(result[2 + offset], 'x');
    BOOST_REQUIRE_EQUAL(result[3 + offset], 't');
}
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_string_length_1) {
    std::deque<uint8_t> result;
    std::string txt("text");

    Packager::append<uint8_t>(result, txt);

    BOOST_REQUIRE_EQUAL(result.size(), 5u);
    BOOST_REQUIRE_EQUAL(result[0], 4u);
    check_text(result, 1);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_string_length_2) {
    std::deque<uint8_t> result;
    std::string txt("text");

    Packager::append<uint16_t>(result, txt);

    BOOST_REQUIRE_EQUAL(result.size(), 6u);
    BOOST_REQUIRE_EQUAL(result[0], 0u);
    BOOST_REQUIRE_EQUAL(result[1], 4u);
    check_text(result, 2);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_string_length_4) {
    std::deque<uint8_t> result;
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
    std::deque<uint8_t> result;
    std::string txt(300,'a');

    BOOST_REQUIRE_THROW(Packager::append<uint8_t>(result, txt), PartsException);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_path_with_two_length_names) {
    std::deque<uint8_t> result;
    boost::filesystem::path path("/usr/share/lib");

    Packager::append(result, path);

    BOOST_REQUIRE_EQUAL(result.size(), 14u + 2u);
    BOOST_REQUIRE_EQUAL(result[2], '/');
    BOOST_REQUIRE_EQUAL(result[6], '/');
    BOOST_REQUIRE_EQUAL(result[7], 's');
    BOOST_REQUIRE_EQUAL(result[15], 'b');
}
