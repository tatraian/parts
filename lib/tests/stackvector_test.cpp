#include <boost/test/auto_unit_test.hpp>

#include "../stackvector.h"
#include "../parts_definitions.h"

using namespace parts;

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_create_buffer) {
    uint8_t buffer[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    StackVector<uint8_t> test(buffer, 10, 4, 2);
    BOOST_CHECK_EQUAL(test.size(), 2);
    BOOST_CHECK_EQUAL(test.capacity(), 10);
    BOOST_CHECK_EQUAL(test.begin(), buffer + 4);
    BOOST_CHECK_EQUAL(test.end(), buffer + 4 + 2);
    BOOST_CHECK_EQUAL(test[0], 4);
    BOOST_CHECK_EQUAL(test[1], 5);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_push_back_element) {
    uint8_t buffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    StackVector<uint8_t> test(buffer, 10, 4, 0);
    test.push_back(5);
    BOOST_CHECK_EQUAL(test.size(), 1);
    BOOST_CHECK_EQUAL(test[0], 5);
    BOOST_CHECK_EQUAL(buffer[4], 5);
    BOOST_CHECK_EQUAL(test.begin() + 1, test.end());
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(front_back_works_properly) {
    uint8_t buffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    StackVector<uint8_t> test(buffer, 10, 4, 0);
    test.push_back(5);
    test.push_back(6);
    BOOST_CHECK_EQUAL(test.front(), 5);
    BOOST_CHECK_EQUAL(test.back(), 6);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(append_works) {
    uint8_t buffer[10] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0};
    std::vector<uint8_t> data = {3,4,5,6};

    StackVector<uint8_t> test(buffer, 10, 2, 1);
    test.append(data.begin(), data.end());
    BOOST_CHECK_EQUAL(test.size(), 5);
    BOOST_CHECK_EQUAL_COLLECTIONS(test.begin(), test.end(), buffer + 2, buffer + 2 + 4 + 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(test.begin() + 1, test.end(), data.begin(), data.end());
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(prepend_works) {
    uint8_t buffer[10] = {0, 0, 0, 0, 0, 0, 0, 7, 0, 0};
    std::vector<uint8_t> data = {3,4,5,6};

    StackVector<uint8_t> test(buffer, 10, 7, 1);
    test.prepend(data.begin(), data.end());
    BOOST_CHECK_EQUAL(test.size(), 5);
    BOOST_CHECK_EQUAL_COLLECTIONS(test.begin(), test.end(), buffer + 7 - 4, buffer + 7 + 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(test.begin(), test.end() - 1, data.begin(), data.end());
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(cannot_erase_from_empty_data) {
    uint8_t buffer[] {0, 0};

    StackVector<uint8_t> test(buffer, 2, 0, 0);
    BOOST_REQUIRE_THROW(test.pop_back(), std::runtime_error);
    BOOST_REQUIRE_THROW(test.pop_front(), std::runtime_error);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throws_if_to_much_data_is_inserted) {
    uint8_t buffer[] = {0};
    std::vector<uint8_t> data = {0,1,2};

    StackVector<uint8_t> test(buffer, 1, 0, 1);
    BOOST_REQUIRE_THROW(test.push_back(0), std::runtime_error);
    BOOST_REQUIRE_THROW(test.push_front(0), std::runtime_error);
    BOOST_REQUIRE_THROW(test.append(data.begin(), data.end()), std::runtime_error);
    BOOST_REQUIRE_THROW(test.prepend(data.begin(), data.end()), std::runtime_error);
}
