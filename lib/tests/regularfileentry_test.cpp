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

    entry.compressEntry(compressor_mock.get(), backend_mock.get());

    Verify(Method(compressor_mock, compressFile)).Once();
    Verify(Method(backend_mock, getPosition)).Once();

    BOOST_REQUIRE_EQUAL(entry.uncompressedSize(), 4);
    BOOST_REQUIRE_EQUAL(entry.compressedSize(), 2);
    BOOST_REQUIRE_EQUAL(entry.offset(), 100);

    std::deque<uint8_t> result;

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


