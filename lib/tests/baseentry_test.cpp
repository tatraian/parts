#include <boost/test/auto_unit_test.hpp>

#include "../baseentry.h"
#include "../parts_definitions.h"

using namespace parts;

namespace
{
// Just to creation
class TestBaseEntry : public BaseEntry {
public:
    TestBaseEntry(const boost::filesystem::path& file,
                  uint16_t permissions,
                  const std::string& owner,
                  uint16_t owner_id,
                  const std::string& group,
                  uint16_t group_id) :
        BaseEntry(file, permissions, owner, owner_id, group, group_id) {}

    TestBaseEntry(std::deque<uint8_t>& buffer, const std::vector<std::string>& owners, const std::vector<std::string>& groups) :
        BaseEntry(buffer, owners, groups) {}

    void compressEntry(const boost::filesystem::path& root, Compressor& compressor, ContentWriteBackend& backend) override {};
    void extractEntry(const boost::filesystem::path& dest_root, Decompressor& decompressor, ContentReadBackend& backend) override {};
    void updateEntry(const BaseEntry* old_entry,
                     const boost::filesystem::path& old_root,
                     const boost::filesystem::path& dest_root,
                     Decompressor& decompressor,
                     ContentReadBackend& backend) override {};

};
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_base_data) {
    TestBaseEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1);

    std::vector<uint8_t> result(100);
    result.clear();
    entry.append(result);

    BOOST_REQUIRE_EQUAL(result.size(), 13u);
    BOOST_REQUIRE_EQUAL(result[0], 0u);
    BOOST_REQUIRE_EQUAL(result[1], 5u);

    BOOST_REQUIRE_EQUAL(result[2], 'f');
    BOOST_REQUIRE_EQUAL(result[6], '1');

    BOOST_REQUIRE_EQUAL(result[7], 1u);
    BOOST_REQUIRE_EQUAL(result[8], 0244);

    BOOST_REQUIRE_EQUAL(result[9], 0u);
    BOOST_REQUIRE_EQUAL(result[10], 2u);

    BOOST_REQUIRE_EQUAL(result[11], 0u);
    BOOST_REQUIRE_EQUAL(result[12], 1u);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_unpack_base_data) {
    std::deque<uint8_t> input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 1};

    std::vector<std::string> names = {"DEFAULT_OWNER", "DEFAULT_GROUP"};

    TestBaseEntry entry(input, names, names);

    BOOST_REQUIRE_EQUAL(input.size(), 0);
    BOOST_CHECK_EQUAL(entry.file(), "file1");
    BOOST_CHECK_EQUAL(entry.permissions(), 0644);
    BOOST_CHECK_EQUAL(entry.owner(), "DEFAULT_OWNER");
    BOOST_CHECK_EQUAL(entry.group(), "DEFAULT_GROUP");
    BOOST_CHECK_EQUAL(entry.ownerId(), 0);
    BOOST_CHECK_EQUAL(entry.groupId(), 1);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throws_if_there_is_no_entry_for_owner) {
    std::deque<uint8_t> input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 3, 0, 1};

    std::vector<std::string> names = {"DEFAULT_OWNER", "DEFAULT_GROUP"};

    BOOST_REQUIRE_THROW(TestBaseEntry entry(input, names, names), PartsException);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(throws_if_there_is_no_entry_for_group) {
    std::deque<uint8_t> input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 3};

    std::vector<std::string> names = {"DEFAULT_OWNER", "DEFAULT_GROUP"};

    BOOST_REQUIRE_THROW(TestBaseEntry entry(input, names, names), PartsException);
}

