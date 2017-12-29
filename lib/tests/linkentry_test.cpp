#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include "../linkentry.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_link_data) {
    LinkEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1, "../file2", true);

    std::vector<uint8_t> result(100);
    result.clear();
    entry.append(result);

    BOOST_CHECK_EQUAL(result.size(), 25u);
    // First the type id
    BOOST_CHECK_EQUAL(result[0], static_cast<uint8_t>(EntryTypes::Link));
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

    // link entry test
    // absolute
    BOOST_REQUIRE_EQUAL(result[14], 1);

    // link name
    BOOST_REQUIRE_EQUAL(result[15], 0u);
    BOOST_REQUIRE_EQUAL(result[16], 8u);

    BOOST_REQUIRE_EQUAL(result[17], '.');
    BOOST_REQUIRE_EQUAL(result[20], 'f');
    BOOST_REQUIRE_EQUAL(result[24], '2');
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(compress_doesn_t_modify_the_output) {
    Mock<ContentWriteBackend> writer_mock;
    Mock<Compressor> compressor_mock;

    // Since mock are not initialized a calling of its function will produce error
    LinkEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1, "../file2", true);

    entry.compressEntry(boost::filesystem::path("nowhere"),  compressor_mock.get(), writer_mock.get());
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_link_data) {
    std::deque<uint8_t> input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 0, 1, 0, 8, '.', '.', '/', 'f', 'i', 'l', 'e', '2'};

    std::vector<std::string> owners = {"DEFAULT_OWNER"};
    std::vector<std::string> groups = {"DEFAULT_GROUP"};

    LinkEntry entry(input, owners, groups);

    BOOST_REQUIRE_EQUAL(input.size(), 0);
    BOOST_CHECK_EQUAL(entry.file(), "file1");
    BOOST_CHECK_EQUAL(entry.permissions(), 0644);
    BOOST_CHECK_EQUAL(entry.owner(), "DEFAULT_OWNER");
    BOOST_CHECK_EQUAL(entry.group(), "DEFAULT_GROUP");
    BOOST_CHECK_EQUAL(entry.ownerId(), 0);
    BOOST_CHECK_EQUAL(entry.groupId(), 0);
    BOOST_CHECK_EQUAL(entry.destination(), "../file2");
    BOOST_CHECK_EQUAL(entry.absolute(), true);

}
