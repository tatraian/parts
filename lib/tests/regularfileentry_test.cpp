#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include "../regularfileentry.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(compress_fills_missing_entries_and_after_it_is_packed_correctly) {
    // Sorry, since it is much to create data entry, I put the two tests in one case...
    Hash hash(HashType::SHA256, {0,1,2,3});
    RegularFileEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1, hash, 4);

    Mock<Compressor> compressor_mock;

    When(Method(compressor_mock, compressFile)).Return(2);

    Mock<ContentWriteBackend> backend_mock;

    When(Method(backend_mock, getPosition)).Return(100);

    entry.compressEntry(boost::filesystem::path("nowhere"), compressor_mock.get(), backend_mock.get());

    Verify(Method(compressor_mock, compressFile)).Once();
    Verify(Method(backend_mock, getPosition)).Once();

    BOOST_REQUIRE_EQUAL(entry.uncompressedSize(), 4);
    BOOST_REQUIRE_EQUAL(entry.compressedSize(), 2);
    BOOST_REQUIRE_EQUAL(entry.offset(), 100);

    std::vector<uint8_t> result;

    entry.append(result);

    BOOST_REQUIRE_EQUAL(result.size(), 70);
    BOOST_REQUIRE_EQUAL(result[0], static_cast<uint8_t>(EntryTypes::RegularFile));

    // base entry check
    BOOST_REQUIRE_EQUAL(result[1], 0u);
    BOOST_REQUIRE_EQUAL(result[2], 5u);

    BOOST_REQUIRE_EQUAL(result[3], 'f');
    BOOST_REQUIRE_EQUAL(result[7], '1');

    BOOST_REQUIRE_EQUAL(result[8], 1u);
    BOOST_REQUIRE_EQUAL(result[9], 0244);

    BOOST_REQUIRE_EQUAL(result[10], 0u);
    BOOST_REQUIRE_EQUAL(result[11], 2u);

    BOOST_REQUIRE_EQUAL(result[12], 0u);
    BOOST_REQUIRE_EQUAL(result[13], 1u);

    // Regular file entry
    // Hash
    BOOST_REQUIRE_EQUAL_COLLECTIONS(result.begin() + 14, result.begin() + 14 + 32, hash.hash().begin(), hash.hash().end());
    // uncompressed size
    BOOST_REQUIRE_EQUAL(result[46], 0);
    BOOST_REQUIRE_EQUAL(result[47], 0);
    BOOST_REQUIRE_EQUAL(result[48], 0);
    BOOST_REQUIRE_EQUAL(result[49], 0);
    BOOST_REQUIRE_EQUAL(result[50], 0);
    BOOST_REQUIRE_EQUAL(result[51], 0);
    BOOST_REQUIRE_EQUAL(result[52], 0);
    BOOST_REQUIRE_EQUAL(result[53], 4);

    // compressed size
    BOOST_REQUIRE_EQUAL(result[54], 0);
    BOOST_REQUIRE_EQUAL(result[55], 0);
    BOOST_REQUIRE_EQUAL(result[56], 0);
    BOOST_REQUIRE_EQUAL(result[57], 0);
    BOOST_REQUIRE_EQUAL(result[58], 0);
    BOOST_REQUIRE_EQUAL(result[59], 0);
    BOOST_REQUIRE_EQUAL(result[60], 0);
    BOOST_REQUIRE_EQUAL(result[61], 2);

    // offset
    BOOST_REQUIRE_EQUAL(result[62], 0);
    BOOST_REQUIRE_EQUAL(result[63], 0);
    BOOST_REQUIRE_EQUAL(result[64], 0);
    BOOST_REQUIRE_EQUAL(result[65], 0);
    BOOST_REQUIRE_EQUAL(result[66], 0);
    BOOST_REQUIRE_EQUAL(result[67], 0);
    BOOST_REQUIRE_EQUAL(result[68], 0);
    BOOST_REQUIRE_EQUAL(result[69], 100);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(regular_file_entry_can_be_created_from_input_stream) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 0,
                                 // hash
                                 0x88, 0xd4, 0x26, 0x6f, 0xd4, 0xe6, 0x33, 0x8d,
                                 0x13, 0xb8, 0x45, 0xfc, 0xf2, 0x89, 0x57, 0x9d,
                                 0x20, 0x9c, 0x89, 0x78, 0x23, 0xb9, 0x21, 0x7d,
                                 0xa3, 0xe1, 0x61, 0x93, 0x6f, 0x03, 0x15, 0x89,
                                 // uncompressed size
                                 0, 0, 0, 0, 0, 0, 0, 100,
                                 // compressed size
                                 0, 0, 0, 0, 0, 0, 0, 42,
                                 // offset
                                 0, 0, 0, 0, 0, 0, 1, 0
                                 };

    std::vector<std::string> owners = {"DEFAULT_OWNER"};
    std::vector<std::string> groups = {"DEFAULT_GROUP"};

    RegularFileEntry entry(input, owners, groups, HashType::SHA256);

    BOOST_REQUIRE_EQUAL(input.size(), 0);
    //base entries
    BOOST_CHECK_EQUAL(entry.file(), "file1");
    BOOST_CHECK_EQUAL(entry.permissions(), 0644);
    BOOST_CHECK_EQUAL(entry.owner(), "DEFAULT_OWNER");
    BOOST_CHECK_EQUAL(entry.group(), "DEFAULT_GROUP");
    BOOST_CHECK_EQUAL(entry.ownerId(), 0);
    BOOST_CHECK_EQUAL(entry.groupId(), 0);

    BOOST_CHECK_EQUAL(entry.uncompressedHash().hashString(), "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
    BOOST_CHECK_EQUAL(entry.uncompressedSize(), 100u);
    BOOST_CHECK_EQUAL(entry.compressedSize(), 42u);
    BOOST_CHECK_EQUAL(entry.offset(), 256u);
}
