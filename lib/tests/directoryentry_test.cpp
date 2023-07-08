#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include "../directoryentry.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;

namespace {
class TestDirectoryEntry : public DirectoryEntry {
public:
    TestDirectoryEntry(const std::filesystem::path& file,
                       uint16_t permissions,
                       const std::string& owner,
                       uint16_t owner_id,
                       const std::string& group,
                       uint16_t group_id) : DirectoryEntry(std::filesystem::path("nowhere"),
                                                           file,
                                                           permissions,
                                                           owner,
                                                           owner_id,
                                                           group,
                                                           group_id)
    {
    }
};
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_directory_data) {
    TestDirectoryEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1);

    std::vector<uint8_t> result(100);
    result.clear();
    entry.append(result);

    BOOST_CHECK_EQUAL(result.size(), 14u);
    // First the type id
    BOOST_CHECK_EQUAL(result[0], static_cast<uint8_t>(EntryTypes::Directory));
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
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(directory_compress_doesn_t_modify_the_output) {
    Mock<ContentWriteBackend> writer_mock;
    // Since mock are not initialized a calling of its function will produce error
    TestDirectoryEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1);

    entry.compressEntry(writer_mock.get());
}
