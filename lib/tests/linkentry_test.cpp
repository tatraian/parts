#include <gmock/gmock.h>
#include <fakeit.hpp>

#include "../linkentry.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;

namespace {
class TestLinkEntry : public LinkEntry {
public:
    TestLinkEntry(const std::filesystem::path& file,
                  uint16_t permissions,
                  const std::string& owner,
                  uint16_t owner_id,
                  const std::string& group,
                  uint16_t group_id,
                  const std::filesystem::path& destination,
                  bool absolute) : LinkEntry(std::filesystem::path("nowhere"),
                                             file,
                                             permissions,
                                             owner,
                                             owner_id,
                                             group,
                                             group_id,
                                             destination,
                                             absolute)
    {
    }
};
}

//==========================================================================================================================================
TEST(link_entry, can_pack_link_data) {
    TestLinkEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1, "../file2", true);

    std::vector<uint8_t> result(100);
    result.clear();
    entry.append(result);

    ASSERT_EQ(result.size(), 24u);
    // First the type id
    ASSERT_EQ(result[0], static_cast<uint8_t>(EntryTypes::Link));
    // base entry check
    ASSERT_EQ(result[1], 0u);
    ASSERT_EQ(result[2], 5u);

    ASSERT_EQ(result[3], 'f');
    ASSERT_EQ(result[7], '1');

    ASSERT_EQ(result[8], 1u);
    ASSERT_EQ(result[9], 0244);

    ASSERT_EQ(result[10], 0u);
    ASSERT_EQ(result[11], 2u);

    ASSERT_EQ(result[12], 0u);
    ASSERT_EQ(result[13], 1u);

    // link entry test
    // link name
    ASSERT_EQ(result[14], 0u);
    ASSERT_EQ(result[15], 8u);

    ASSERT_EQ(result[16], '.');
    ASSERT_EQ(result[19], 'f');
    ASSERT_EQ(result[23], '2');
}

//==========================================================================================================================================
TEST(link_entry, compress_doesn_t_modify_the_output) {
    Mock<ContentWriteBackend> writer_mock;

    // Since mock are not initialized a calling of its function will produce error
    TestLinkEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1, "../file2", true);

    entry.compressEntry(writer_mock.get());
}


//==========================================================================================================================================
TEST(link_entry, can_unpack_link_data) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 0, 0, 8, '.', '.', '/', 'f', 'i', 'l', 'e', '2'};

    std::vector<std::string> owners = {"DEFAULT_OWNER"};
    std::vector<std::string> groups = {"DEFAULT_GROUP"};

    LinkEntry entry(input, owners, groups);

    ASSERT_EQ(input.size(), 0);
    ASSERT_EQ(entry.file(), "file1");
    ASSERT_EQ(entry.permissions(), 0644);
    ASSERT_EQ(entry.owner(), "DEFAULT_OWNER");
    ASSERT_EQ(entry.group(), "DEFAULT_GROUP");
    ASSERT_EQ(entry.ownerId(), 0);
    ASSERT_EQ(entry.groupId(), 0);
    ASSERT_EQ(entry.destination(), "../file2");
}

//==========================================================================================================================================
TEST(link_entry, don_t_throw_exception_in_case_of_read_error) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 0, 0, 8, '.', '.', '/', 'f', 'i', 'l', 'e'};

    std::vector<std::string> owners = {"DEFAULT_OWNER"};
    std::vector<std::string> groups = {"DEFAULT_GROUP"};

    LinkEntry entry(input, owners, groups);
    ASSERT_TRUE(!entry);

}
