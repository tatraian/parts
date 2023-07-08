#include <boost/test/auto_unit_test.hpp>

#include <filesystem>

#include "../hash.h"

using namespace parts;

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(hash_do_not_throw_exception_in_case_of_not_existing_files) {
    std::filesystem::path not_exist("/not/existing/file/with/long/name");

    Hash h(HashType::SHA256, not_exist);
    BOOST_REQUIRE(!h.isValid());
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(hash_works_md5) {
    std::vector<uint8_t> data = {'a', 'b', 'c', 'd'};
    Hash hash(HashType::MD5, data);

    BOOST_REQUIRE_EQUAL(hash.hashString(), "e2fc714c4727ee9395f324cd2e7f331f");
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(hashing_works) {
    std::vector<uint8_t> data = {'a', 'b', 'c', 'd'};
    Hash hash(HashType::SHA256, data);

    BOOST_REQUIRE_EQUAL(hash.hashString(), "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(hash_can_initialized_from_stream) {
    InputBuffer input = {0x88, 0xd4, 0x26, 0x6f, 0xd4, 0xe6, 0x33, 0x8d,
                                 0x13, 0xb8, 0x45, 0xfc, 0xf2, 0x89, 0x57, 0x9d,
                                 0x20, 0x9c, 0x89, 0x78, 0x23, 0xb9, 0x21, 0x7d,
                                 0xa3, 0xe1, 0x61, 0x93, 0x6f, 0x03, 0x15, 0x89};

    Hash hash(HashType::SHA256, input);
    BOOST_REQUIRE(hash.isValid());
    BOOST_CHECK(input.empty());
    BOOST_CHECK(hash.type() == HashType::SHA256);
    BOOST_CHECK_EQUAL(hash.hashString(), "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(hash_don_t_trows_exception_in_case_of_read_error) {
    InputBuffer input = {0x88, 0xd4, 0x26, 0x6f, 0xd4, 0xe6, 0x33, 0x8d,
                                 0x13, 0xb8, 0x45, 0xfc, 0xf2, 0x89, 0x57, 0x9d};

    Hash hash(HashType::SHA256, input);
    BOOST_REQUIRE(!hash.isValid());

}
