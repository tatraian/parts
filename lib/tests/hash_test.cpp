#include <boost/test/auto_unit_test.hpp>

#include <boost/filesystem.hpp>

#include "../hash.h"

using namespace parts;

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(hash_throws_exception_in_case_of_not_existing_files) {
    boost::filesystem::path not_exist("/not/existing/file/with/long/name");

    BOOST_REQUIRE_EXCEPTION(Hash(HashType::SHA256, not_exist),
                            PartsException,
                            [&](const PartsException& e){return e.what() == std::string("File doesn't exist: " + not_exist.string());});
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
    BOOST_CHECK(input.empty());
    BOOST_CHECK(hash.type() == HashType::SHA256);
    BOOST_CHECK_EQUAL(hash.hashString(), "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
}

BOOST_AUTO_TEST_CASE(hash_can_initialized_from_string) {
    std::vector<uint8_t> result = {0x88, 0xd4, 0x26, 0x6f, 0xd4, 0xe6, 0x33, 0x8d,
                                   0x13, 0xb8, 0x45, 0xfc, 0xf2, 0x89, 0x57, 0x9d,
                                   0x20, 0x9c, 0x89, 0x78, 0x23, 0xb9, 0x21, 0x7d,
                                   0xa3, 0xe1, 0x61, 0x93, 0x6f, 0x03, 0x15, 0x89};

    std::string hash_string = "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589";
    Hash hash(HashType::SHA256, hash_string.begin(), hash_string.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(hash.hash().begin(), hash.hash().end(), result.begin(), result.end());
}
