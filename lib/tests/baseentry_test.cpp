#include <gmock/gmock.h>

#include "../baseentry.h"
#include "../parts_definitions.h"

using namespace parts;

namespace
{
// Just to creation
class TestBaseEntry : public BaseEntry {
public:
    TestBaseEntry(const std::filesystem::path& file,
                  uint16_t permissions,
                  const std::string& owner,
                  uint16_t owner_id,
                  const std::string& group,
                  uint16_t group_id) :
        BaseEntry("nowhere", file, permissions, owner, owner_id, group, group_id) {}

    TestBaseEntry(InputBuffer& buffer, const std::vector<std::string>& owners, const std::vector<std::string>& groups) :
        BaseEntry(buffer, owners, groups) {}

    void compressEntry(ContentWriteBackend& backend) override {}
    void extractEntry(const std::filesystem::path& dest_root, ContentReadBackend& backend, bool cont) override {}
    void updateEntry(const BaseEntry* old_entry,
                     const std::filesystem::path& old_root,
                     const std::filesystem::path& dest_root,
                     ContentReadBackend& backend,
                     bool cont) override {}

    std::string listEntry(size_t user_width, size_t size_width, std::tm* t) const override { return ""; }
};
}


//==========================================================================================================================================
TEST(base_entry, can_pack_base_data) {
    TestBaseEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1);
    ASSERT_TRUE(entry);

    std::vector<uint8_t> result(100);
    result.clear();
    entry.append(result);

    ASSERT_EQ(result.size(), 13u);
    ASSERT_EQ(result[0], 0u);
    ASSERT_EQ(result[1], 5u);

    ASSERT_EQ(result[2], 'f');
    ASSERT_EQ(result[6], '1');

    ASSERT_EQ(result[7], 1u);
    ASSERT_EQ(result[8], 0244);

    ASSERT_EQ(result[9], 0u);
    ASSERT_EQ(result[10], 2u);

    ASSERT_EQ(result[11], 0u);
    ASSERT_EQ(result[12], 1u);
}

//==========================================================================================================================================
TEST(base_entry, can_unpack_base_data) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 1};

    std::vector<std::string> names = {"DEFAULT_OWNER", "DEFAULT_GROUP"};

    TestBaseEntry entry(input, names, names);

    ASSERT_TRUE(entry);
    ASSERT_EQ(input.size(), 0);
    ASSERT_EQ(entry.file(), "file1");
    ASSERT_EQ(entry.permissions(), 0644);
    ASSERT_EQ(entry.owner(), "DEFAULT_OWNER");
    ASSERT_EQ(entry.group(), "DEFAULT_GROUP");
    ASSERT_EQ(entry.ownerId(), 0);
    ASSERT_EQ(entry.groupId(), 1);
}

//==========================================================================================================================================
TEST(base_entry, throws_if_there_is_no_entry_for_owner) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 3, 0, 1};

    std::vector<std::string> names = {"DEFAULT_OWNER", "DEFAULT_GROUP"};

    TestBaseEntry entry(input, names, names);
    ASSERT_TRUE(!entry);
}

//==========================================================================================================================================
TEST(base_entry, throws_if_there_is_no_entry_for_group) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 3};

    std::vector<std::string> names = {"DEFAULT_OWNER", "DEFAULT_GROUP"};

    TestBaseEntry entry(input, names, names);
    ASSERT_TRUE(!entry);
}

//==========================================================================================================================================
TEST(base_entry, will_be_invalid_in_case_of_read_error) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 3};

    std::vector<std::string> names = {"DEFAULT_OWNER", "DEFAULT_GROUP"};

    TestBaseEntry entry(input, names, names);
    ASSERT_TRUE(!entry);

}

