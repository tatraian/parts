#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

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
class TestRegularFileEntry : public RegularFileEntry {
public:
    TestRegularFileEntry() : RegularFileEntry("nowhere", "file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1, PartsCompressionParameters())
    {
        m_uncompressedHash = Hash(HashType::SHA256, {0,1,2,3});
        m_uncompressedSize = 4;
    }

    std::unique_ptr<Compressor> createCompressor() {
        return std::unique_ptr<Compressor>(new FakeCompressor());
    }

};

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(compress_fills_missing_entries_and_after_it_is_packed_correctly, TestRegularFileEntry) {
    // Sorry, since it is much to create data entry, I put the two tests in one case...
    Hash hash(HashType::SHA256, {0,1,2,3});

    Mock<ContentWriteBackend> backend_mock;
    When(Method(backend_mock, getPosition)).Return(100);

    compressEntry(backend_mock.get());

    Verify(Method(backend_mock, getPosition)).Once();

    BOOST_REQUIRE(m_compressionType == CompressionType::LZMA);
    BOOST_REQUIRE_EQUAL(m_uncompressedSize, 4);
    BOOST_REQUIRE_EQUAL(m_compressedSize, 2);
    BOOST_REQUIRE_EQUAL(m_offset, 100);

    std::vector<uint8_t> result;

    append(result);

    BOOST_REQUIRE_EQUAL(result.size(), 71);
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
    // compression type
    BOOST_REQUIRE_EQUAL(result[46], 1);
    // uncompressed size
    BOOST_REQUIRE_EQUAL(result[47], 0);
    BOOST_REQUIRE_EQUAL(result[48], 0);
    BOOST_REQUIRE_EQUAL(result[49], 0);
    BOOST_REQUIRE_EQUAL(result[50], 0);
    BOOST_REQUIRE_EQUAL(result[51], 0);
    BOOST_REQUIRE_EQUAL(result[52], 0);
    BOOST_REQUIRE_EQUAL(result[53], 0);
    BOOST_REQUIRE_EQUAL(result[54], 4);

    // compressed size
    BOOST_REQUIRE_EQUAL(result[55], 0);
    BOOST_REQUIRE_EQUAL(result[56], 0);
    BOOST_REQUIRE_EQUAL(result[57], 0);
    BOOST_REQUIRE_EQUAL(result[58], 0);
    BOOST_REQUIRE_EQUAL(result[59], 0);
    BOOST_REQUIRE_EQUAL(result[60], 0);
    BOOST_REQUIRE_EQUAL(result[61], 0);
    BOOST_REQUIRE_EQUAL(result[62], 2);

    // offset
    BOOST_REQUIRE_EQUAL(result[63], 0);
    BOOST_REQUIRE_EQUAL(result[64], 0);
    BOOST_REQUIRE_EQUAL(result[65], 0);
    BOOST_REQUIRE_EQUAL(result[66], 0);
    BOOST_REQUIRE_EQUAL(result[67], 0);
    BOOST_REQUIRE_EQUAL(result[68], 0);
    BOOST_REQUIRE_EQUAL(result[69], 0);
    BOOST_REQUIRE_EQUAL(result[70], 100);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(regular_file_entry_can_be_created_from_input_stream) {
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

    BOOST_REQUIRE_EQUAL(input.size(), 0);
    //base entries
    BOOST_CHECK_EQUAL(entry.file(), "file1");
    BOOST_CHECK_EQUAL(entry.permissions(), 0644);
    BOOST_CHECK_EQUAL(entry.owner(), "DEFAULT_OWNER");
    BOOST_CHECK_EQUAL(entry.group(), "DEFAULT_GROUP");
    BOOST_CHECK_EQUAL(entry.ownerId(), 0);
    BOOST_CHECK_EQUAL(entry.groupId(), 0);

    BOOST_CHECK(entry.compressionType() == CompressionType::LZMA);
    BOOST_CHECK_EQUAL(entry.uncompressedHash().hashString(), "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
    BOOST_CHECK_EQUAL(entry.uncompressedSize(), 100u);
    BOOST_CHECK_EQUAL(entry.compressedSize(), 42u);
    BOOST_CHECK_EQUAL(entry.offset(), 256u);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(regular_file_entry_don_t_throw_if_there_is_no_enough_data_at_hash) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 0,
                                 // hash
                                 0x88, 0xd4, 0x26, 0x6f, 0xd4, 0xe6, 0x33, 0x8d,
                                 0x13, 0xb8, 0x45, 0xfc, 0xf2, 0x89, 0x57, 0x9d,
                                 0x20, 0x9c, 0x89, 0x78, 0x23, 0xb9, 0x21, 0x7d,
                                 };

    std::vector<std::string> owners = {"DEFAULT_OWNER"};
    std::vector<std::string> groups = {"DEFAULT_GROUP"};

    RegularFileEntry entry(input, owners, groups, HashType::SHA256);
    BOOST_CHECK(!entry);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(regular_file_entry_don_t_throw_if_there_is_no_enough_ragular_file_specific_data) {
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
    BOOST_CHECK(!entry);
}

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(names_in_tables_are_find_correctly, TestRegularFileEntry) {
    std::vector<std::string> owners;

    BOOST_REQUIRE_EQUAL(findOrInsert("aaa", owners), 0);
    BOOST_REQUIRE_EQUAL(findOrInsert("aaa", owners), 0);
    BOOST_REQUIRE_EQUAL(findOrInsert("bbb", owners), 1);
}

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(owner_will_be_default_if_owner_saving_is_disabled, TestRegularFileEntry) {
    std::vector<std::string> owners;
    owners.push_back("default");

    struct stat nothing;
    BOOST_REQUIRE_EQUAL(getOwnerId(&nothing, owners, false), 0);
}


//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(group_will_be_default_if_owner_saving_is_disabled, TestRegularFileEntry) {
    std::vector<std::string> owners;
    owners.push_back("default");

    struct stat nothing;
    BOOST_REQUIRE_EQUAL(getGroupId(&nothing, owners, false), 0);
}


//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(permessions_are_saved_correctly, TestRegularFileEntry) {
    struct stat file_stat;

    // If this test fails than your OP represents different ways the permissions. So we won't work there...
    file_stat.st_mode = 040755; //drwxr-xr-x
    uint16_t perms = getPermissions(&file_stat);
    BOOST_CHECK((perms & S_ISUID) == 0);
    BOOST_CHECK((perms & S_ISGID) == 0);
    BOOST_CHECK(perms & S_IRUSR);
    BOOST_CHECK(perms & S_IWUSR);
    BOOST_CHECK(perms & S_IXUSR);

    BOOST_CHECK(perms & S_IRGRP);
    BOOST_CHECK((perms & S_IWGRP) == 0);
    BOOST_CHECK(perms & S_IXGRP);

    BOOST_CHECK(perms & S_IROTH);
    BOOST_CHECK((perms & S_IWOTH) == 0);
    BOOST_CHECK(perms & S_IXOTH);

    file_stat.st_mode = 0100644; //-rw-r--r--
    perms = getPermissions(&file_stat);
    BOOST_CHECK((perms & S_ISUID) == 0);
    BOOST_CHECK((perms & S_ISGID) == 0);
    BOOST_CHECK(perms & S_IRUSR);
    BOOST_CHECK(perms & S_IWUSR);
    BOOST_CHECK((perms & S_IXUSR) == 0);

    BOOST_CHECK(perms & S_IRGRP);
    BOOST_CHECK((perms & S_IWGRP) == 0);
    BOOST_CHECK((perms & S_IXGRP) == 0);

    BOOST_CHECK(perms & S_IROTH);
    BOOST_CHECK((perms & S_IWOTH) == 0);
    BOOST_CHECK((perms & S_IXOTH) == 0);

    file_stat.st_mode = 0100755;//-rwxr-xr-x
    perms = getPermissions(&file_stat);
    BOOST_CHECK((perms & S_ISUID) == 0);
    BOOST_CHECK((perms & S_ISGID) == 0);
    BOOST_CHECK(perms & S_IRUSR);
    BOOST_CHECK(perms & S_IWUSR);
    BOOST_CHECK(perms & S_IXUSR);

    BOOST_CHECK(perms & S_IRGRP);
    BOOST_CHECK((perms & S_IWGRP) == 0);
    BOOST_CHECK(perms & S_IXGRP);

    BOOST_CHECK(perms & S_IROTH);
    BOOST_CHECK((perms & S_IWOTH) == 0);
    BOOST_CHECK(perms & S_IXOTH);

    file_stat.st_mode = 0104755; //-rwxr-xr-x with SUID
    perms = getPermissions(&file_stat);
    BOOST_CHECK(perms & S_ISUID);
    BOOST_CHECK((perms & S_ISGID) == 0);
    BOOST_CHECK(perms & S_IRUSR);
    BOOST_CHECK(perms & S_IWUSR);
    BOOST_CHECK(perms & S_IXUSR);

    BOOST_CHECK(perms & S_IRGRP);
    BOOST_CHECK((perms & S_IWGRP) == 0);
    BOOST_CHECK(perms & S_IXGRP);

    BOOST_CHECK(perms & S_IROTH);
    BOOST_CHECK((perms & S_IWOTH) == 0);
    BOOST_CHECK(perms & S_IXOTH);

    file_stat.st_mode = 0102755; //-rwxr-xr-x with GUID
    perms = getPermissions(&file_stat);
    BOOST_CHECK((perms & S_ISUID) == 0);
    BOOST_CHECK(perms & S_ISGID);
    BOOST_CHECK(perms & S_IRUSR);
    BOOST_CHECK(perms & S_IWUSR);
    BOOST_CHECK(perms & S_IXUSR);

    BOOST_CHECK(perms & S_IRGRP);
    BOOST_CHECK((perms & S_IWGRP) == 0);
    BOOST_CHECK(perms & S_IXGRP);

    BOOST_CHECK(perms & S_IROTH);
    BOOST_CHECK((perms & S_IWOTH) == 0);
    BOOST_CHECK(perms & S_IXOTH);

    file_stat.st_mode = 0120777; //softlink
    perms = getPermissions(&file_stat);
    BOOST_CHECK((perms & S_ISUID) == 0);
    BOOST_CHECK((perms & S_ISGID) == 0);
    BOOST_CHECK(perms & S_IRUSR);
    BOOST_CHECK(perms & S_IWUSR);
    BOOST_CHECK(perms & S_IXUSR);

    BOOST_CHECK(perms & S_IRGRP);
    BOOST_CHECK(perms & S_IWGRP);
    BOOST_CHECK(perms & S_IXGRP);

    BOOST_CHECK(perms & S_IROTH);
    BOOST_CHECK(perms & S_IWOTH);
    BOOST_CHECK(perms & S_IXOTH);
}


