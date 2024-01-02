#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fakeit.hpp>

#include "../regularfileentry.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;

//==========================================================================================================================================
class FakeCompressor : public Compressor {
public:
    FakeCompressor() : Compressor() {}

    size_t compressFile(const std::filesystem::path& path,
                        ContentWriteBackend& backend) override {
        return 2;
    }

    size_t compressBuffer(const std::vector<uint8_t>& buffer,
                          std::vector<uint8_t>& backend) override { return 0; }

};

//==========================================================================================================================================
class TestRegularFileEntry : public RegularFileEntry, public ::testing::Test {
public:
    TestRegularFileEntry() : RegularFileEntry("nowhere", "file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1, PartsCompressionParameters())
    {
        m_uncompressedHash = Hash(HashType::SHA256, std::vector<uint8_t>{0,1,2,3});
        m_uncompressedSize = 4;
    }

    std::unique_ptr<Compressor> createCompressor() {
        return std::unique_ptr<Compressor>(new FakeCompressor());
    }

};

//==========================================================================================================================================
TEST_F(TestRegularFileEntry, compress_fills_missing_entries_and_after_it_is_packed_correctly) {
    // Sorry, since it is much to create data entry, I put the two tests in one case...
    Hash hash(HashType::SHA256, std::vector<uint8_t>{0,1,2,3});

    Mock<ContentWriteBackend> backend_mock;
    When(Method(backend_mock, getPosition)).Return(100);

    compressEntry(backend_mock.get());

    Verify(Method(backend_mock, getPosition)).Once();

    ASSERT_TRUE(m_compressionType == CompressionType::LZMA);
    ASSERT_EQ(m_uncompressedSize, 4);
    ASSERT_EQ(m_compressedSize, 2);
    ASSERT_EQ(m_offset, 100);

    std::vector<uint8_t> result;

    append(result);

    ASSERT_EQ(result.size(), 71);
    ASSERT_EQ(result[0], static_cast<uint8_t>(EntryTypes::RegularFile));

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

    // Regular file entry
    // Hash
    std::vector<uint8_t> checked_part(result.begin() + 14, result.begin() + 14 + 32);
    ASSERT_THAT(checked_part, hash.hash());
    //ASSERT_EQ_COLLECTIONS(result.begin() + 14, result.begin() + 14 + 32, hash.hash().begin(), hash.hash().end());
    // compression type
    ASSERT_EQ(result[46], 1);
    // uncompressed size
    ASSERT_EQ(result[47], 0);
    ASSERT_EQ(result[48], 0);
    ASSERT_EQ(result[49], 0);
    ASSERT_EQ(result[50], 0);
    ASSERT_EQ(result[51], 0);
    ASSERT_EQ(result[52], 0);
    ASSERT_EQ(result[53], 0);
    ASSERT_EQ(result[54], 4);

    // compressed size
    ASSERT_EQ(result[55], 0);
    ASSERT_EQ(result[56], 0);
    ASSERT_EQ(result[57], 0);
    ASSERT_EQ(result[58], 0);
    ASSERT_EQ(result[59], 0);
    ASSERT_EQ(result[60], 0);
    ASSERT_EQ(result[61], 0);
    ASSERT_EQ(result[62], 2);

    // offset
    ASSERT_EQ(result[63], 0);
    ASSERT_EQ(result[64], 0);
    ASSERT_EQ(result[65], 0);
    ASSERT_EQ(result[66], 0);
    ASSERT_EQ(result[67], 0);
    ASSERT_EQ(result[68], 0);
    ASSERT_EQ(result[69], 0);
    ASSERT_EQ(result[70], 100);
}

//==========================================================================================================================================
TEST(regular_file_entry, regular_file_entry_can_be_created_from_input_stream) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 0,
                                 // hash
                                 0x88, 0xd4, 0x26, 0x6f, 0xd4, 0xe6, 0x33, 0x8d,
                                 0x13, 0xb8, 0x45, 0xfc, 0xf2, 0x89, 0x57, 0x9d,
                                 0x20, 0x9c, 0x89, 0x78, 0x23, 0xb9, 0x21, 0x7d,
                                 0xa3, 0xe1, 0x61, 0x93, 0x6f, 0x03, 0x15, 0x89,
                                 // compression type
                                 0x01, // LZMA
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

    ASSERT_EQ(input.size(), 0);
    //base entries
    ASSERT_EQ(entry.file(), "file1");
    ASSERT_EQ(entry.permissions(), 0644);
    ASSERT_EQ(entry.owner(), "DEFAULT_OWNER");
    ASSERT_EQ(entry.group(), "DEFAULT_GROUP");
    ASSERT_EQ(entry.ownerId(), 0);
    ASSERT_EQ(entry.groupId(), 0);

    ASSERT_TRUE(entry.compressionType() == CompressionType::LZMA);
    ASSERT_EQ(entry.uncompressedHash().hashString(), "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
    ASSERT_EQ(entry.uncompressedSize(), 100u);
    ASSERT_EQ(entry.compressedSize(), 42u);
    ASSERT_EQ(entry.offset(), 256u);
}


//==========================================================================================================================================
TEST(regular_file_entry, regular_file_entry_don_t_throw_if_there_is_no_enough_data_at_hash) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 0,
                                 // hash
                                 0x88, 0xd4, 0x26, 0x6f, 0xd4, 0xe6, 0x33, 0x8d,
                                 0x13, 0xb8, 0x45, 0xfc, 0xf2, 0x89, 0x57, 0x9d,
                                 0x20, 0x9c, 0x89, 0x78, 0x23, 0xb9, 0x21, 0x7d,
                                 };

    std::vector<std::string> owners = {"DEFAULT_OWNER"};
    std::vector<std::string> groups = {"DEFAULT_GROUP"};

    RegularFileEntry entry(input, owners, groups, HashType::SHA256);
    ASSERT_TRUE(!entry);
}

//==========================================================================================================================================
TEST(regular_file_entry, regular_file_entry_don_t_throw_if_there_is_no_enough_ragular_file_specific_data) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 0,
                                 // hash
                                 0x88, 0xd4, 0x26, 0x6f, 0xd4, 0xe6, 0x33, 0x8d,
                                 0x13, 0xb8, 0x45, 0xfc, 0xf2, 0x89, 0x57, 0x9d,
                                 0x20, 0x9c, 0x89, 0x78, 0x23, 0xb9, 0x21, 0x7d,
                                 0xa3, 0xe1, 0x61, 0x93, 0x6f, 0x03, 0x15, 0x89,
                                 // compression type
                                 0x01, // LZMA
                                 // uncompressed size
                                 0, 0, 0, 0, 0, 0, 0, 100,
                                 // compressed size
                                 0, 0, 0, 0, 0, 0, 0, 42,
                                 // offset
                                 0, 0, 0, 0, 0, 0
                                 };

    std::vector<std::string> owners = {"DEFAULT_OWNER"};
    std::vector<std::string> groups = {"DEFAULT_GROUP"};

    RegularFileEntry entry(input, owners, groups, HashType::SHA256);
    ASSERT_TRUE(!entry);
}

//==========================================================================================================================================
TEST_F(TestRegularFileEntry, names_in_tables_are_find_correctly) {
    std::vector<std::string> owners;

    ASSERT_EQ(findOrInsert("aaa", owners), 0);
    ASSERT_EQ(findOrInsert("aaa", owners), 0);
    ASSERT_EQ(findOrInsert("bbb", owners), 1);
}

//==========================================================================================================================================
TEST_F(TestRegularFileEntry, owner_will_be_default_if_owner_saving_is_disabled) {
    std::vector<std::string> owners;
    owners.push_back("default");

    struct stat nothing;
    ASSERT_EQ(getOwnerId(&nothing, owners, false), 0);
}


//==========================================================================================================================================
TEST_F(TestRegularFileEntry, group_will_be_default_if_owner_saving_is_disabled) {
    std::vector<std::string> owners;
    owners.push_back("default");

    struct stat nothing;
    ASSERT_EQ(getGroupId(&nothing, owners, false), 0);
}


//==========================================================================================================================================
TEST_F(TestRegularFileEntry, permessions_are_saved_correctly) {
    struct stat file_stat;

    // If this test fails than your OP represents different ways the permissions. So we won't work there...
    file_stat.st_mode = 040755; //drwxr-xr-x
    uint16_t perms = getPermissions(&file_stat);
    ASSERT_TRUE((perms & S_ISUID) == 0);
    ASSERT_TRUE((perms & S_ISGID) == 0);
    ASSERT_TRUE(perms & S_IRUSR);
    ASSERT_TRUE(perms & S_IWUSR);
    ASSERT_TRUE(perms & S_IXUSR);

    ASSERT_TRUE(perms & S_IRGRP);
    ASSERT_TRUE((perms & S_IWGRP) == 0);
    ASSERT_TRUE(perms & S_IXGRP);

    ASSERT_TRUE(perms & S_IROTH);
    ASSERT_TRUE((perms & S_IWOTH) == 0);
    ASSERT_TRUE(perms & S_IXOTH);

    file_stat.st_mode = 0100644; //-rw-r--r--
    perms = getPermissions(&file_stat);
    ASSERT_TRUE((perms & S_ISUID) == 0);
    ASSERT_TRUE((perms & S_ISGID) == 0);
    ASSERT_TRUE(perms & S_IRUSR);
    ASSERT_TRUE(perms & S_IWUSR);
    ASSERT_TRUE((perms & S_IXUSR) == 0);

    ASSERT_TRUE(perms & S_IRGRP);
    ASSERT_TRUE((perms & S_IWGRP) == 0);
    ASSERT_TRUE((perms & S_IXGRP) == 0);

    ASSERT_TRUE(perms & S_IROTH);
    ASSERT_TRUE((perms & S_IWOTH) == 0);
    ASSERT_TRUE((perms & S_IXOTH) == 0);

    file_stat.st_mode = 0100755;//-rwxr-xr-x
    perms = getPermissions(&file_stat);
    ASSERT_TRUE((perms & S_ISUID) == 0);
    ASSERT_TRUE((perms & S_ISGID) == 0);
    ASSERT_TRUE(perms & S_IRUSR);
    ASSERT_TRUE(perms & S_IWUSR);
    ASSERT_TRUE(perms & S_IXUSR);

    ASSERT_TRUE(perms & S_IRGRP);
    ASSERT_TRUE((perms & S_IWGRP) == 0);
    ASSERT_TRUE(perms & S_IXGRP);

    ASSERT_TRUE(perms & S_IROTH);
    ASSERT_TRUE((perms & S_IWOTH) == 0);
    ASSERT_TRUE(perms & S_IXOTH);

    file_stat.st_mode = 0104755; //-rwxr-xr-x with SUID
    perms = getPermissions(&file_stat);
    ASSERT_TRUE(perms & S_ISUID);
    ASSERT_TRUE((perms & S_ISGID) == 0);
    ASSERT_TRUE(perms & S_IRUSR);
    ASSERT_TRUE(perms & S_IWUSR);
    ASSERT_TRUE(perms & S_IXUSR);

    ASSERT_TRUE(perms & S_IRGRP);
    ASSERT_TRUE((perms & S_IWGRP) == 0);
    ASSERT_TRUE(perms & S_IXGRP);

    ASSERT_TRUE(perms & S_IROTH);
    ASSERT_TRUE((perms & S_IWOTH) == 0);
    ASSERT_TRUE(perms & S_IXOTH);

    file_stat.st_mode = 0102755; //-rwxr-xr-x with GUID
    perms = getPermissions(&file_stat);
    ASSERT_TRUE((perms & S_ISUID) == 0);
    ASSERT_TRUE(perms & S_ISGID);
    ASSERT_TRUE(perms & S_IRUSR);
    ASSERT_TRUE(perms & S_IWUSR);
    ASSERT_TRUE(perms & S_IXUSR);

    ASSERT_TRUE(perms & S_IRGRP);
    ASSERT_TRUE((perms & S_IWGRP) == 0);
    ASSERT_TRUE(perms & S_IXGRP);

    ASSERT_TRUE(perms & S_IROTH);
    ASSERT_TRUE((perms & S_IWOTH) == 0);
    ASSERT_TRUE(perms & S_IXOTH);

    file_stat.st_mode = 0120777; //softlink
    perms = getPermissions(&file_stat);
    ASSERT_TRUE((perms & S_ISUID) == 0);
    ASSERT_TRUE((perms & S_ISGID) == 0);
    ASSERT_TRUE(perms & S_IRUSR);
    ASSERT_TRUE(perms & S_IWUSR);
    ASSERT_TRUE(perms & S_IXUSR);

    ASSERT_TRUE(perms & S_IRGRP);
    ASSERT_TRUE(perms & S_IWGRP);
    ASSERT_TRUE(perms & S_IXGRP);

    ASSERT_TRUE(perms & S_IROTH);
    ASSERT_TRUE(perms & S_IWOTH);
    ASSERT_TRUE(perms & S_IXOTH);
}


