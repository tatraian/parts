#include <gmock/gmock.h>
#include <fakeit.hpp>

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
TEST(directory_entry, can_pack_directory_data) {
    TestDirectoryEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1);

    std::vector<uint8_t> result(100);
    result.clear();
    entry.append(result);

    ASSERT_EQ(result.size(), 14u);
    // First the type id
    ASSERT_EQ(result[0], static_cast<uint8_t>(EntryTypes::Directory));
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
}


//==========================================================================================================================================
TEST(directory_entry, directory_compress_doesn_t_modify_the_output) {
    Mock<ContentWriteBackend> writer_mock;
    // Since mock are not initialized a calling of its function will produce error
    TestDirectoryEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1);

    entry.compressEntry(writer_mock.get());
}
