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
BOOST_AUTO_TEST_CASE(only_sha256_is_supported_currently) {
    std::vector<uint8_t> data(100);

    BOOST_REQUIRE_EXCEPTION(Hash(HashType::MD5, data),
                            PartsException,
                            [&](const PartsException& e){return e.what() == std::string("Only SHA 256 hash is supported currently");});
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(hashing_works) {
    std::vector<uint8_t> data = {'a', 'b', 'c', 'd'};
    Hash hash(HashType::SHA256, data);

    BOOST_REQUIRE_EQUAL(hash.hashString(), "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
}
