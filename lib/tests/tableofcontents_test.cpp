#define BOOST_TEST_MODULE parts_test
#include <boost/test/auto_unit_test.hpp>

#include "../tableofcontents.h"
#include "../parts_definitions.h"
#include "../regularfileentry.h"
#include "../linkentry.h"
#include "../directoryentry.h"

#include <boost/filesystem.hpp>

#include <iostream>

using namespace parts;

namespace {
PartsCompressionParameters static_parameters;
}

class FakeTableOfContents : public TableOfContents {
public:
    FakeTableOfContents() : TableOfContents(static_parameters)
    {}
};

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(TableOfContents_can_parse_only_one_file) {
    const char* filename = "/tmp/one_file";
    PartsCompressionParameters params;

    boost::filesystem::remove(filename);
    std::ofstream file(filename, std::ios::ate);

    BOOST_REQUIRE(boost::filesystem::is_regular_file(filename));

    TableOfContents toc(filename, params);

    BOOST_REQUIRE_EQUAL(toc.size(), 1);
    BOOST_REQUIRE_EQUAL(toc.begin()->first, boost::filesystem::path(filename).filename());
    std::shared_ptr<BaseEntry> entry = toc.begin()->second;
    std::shared_ptr<RegularFileEntry> file_entry = std::dynamic_pointer_cast<RegularFileEntry>(entry);
    BOOST_REQUIRE(file_entry);
    BOOST_REQUIRE_EQUAL(file_entry->file(), boost::filesystem::path(filename).filename());

    BOOST_REQUIRE_EQUAL(file_entry->ownerId(), 0);
    BOOST_REQUIRE_EQUAL(file_entry->owner(), TableOfContents::DEFAULT_OWNER);
    BOOST_REQUIRE_EQUAL(file_entry->groupId(), 0);
    BOOST_REQUIRE_EQUAL(file_entry->group(), TableOfContents::DEFAULT_GROUP);
}

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(names_in_tables_are_find_correctly, FakeTableOfContents) {
    BOOST_REQUIRE(m_owners.empty());

    BOOST_REQUIRE_EQUAL(findOrInsert("aaa", m_owners), 0);
    BOOST_REQUIRE_EQUAL(findOrInsert("aaa", m_owners), 0);
    BOOST_REQUIRE_EQUAL(findOrInsert("bbb", m_owners), 1);
}

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(owner_will_be_default_if_owner_saving_is_disabled, FakeTableOfContents) {
    m_owners.push_back(TableOfContents::DEFAULT_OWNER);
    PartsCompressionParameters params;
    static_parameters = params;
    static_parameters.m_saveOwners = false;

    struct stat nothing;
    BOOST_REQUIRE_EQUAL(getOwnerId(&nothing), 0);
}

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(group_will_be_default_if_owner_saving_is_disabled, FakeTableOfContents) {
    m_owners.push_back(TableOfContents::DEFAULT_GROUP);
    PartsCompressionParameters params;
    static_parameters = params;
    static_parameters.m_saveOwners = false;

    struct stat nothing;
    BOOST_REQUIRE_EQUAL(getGroupId(&nothing), 0);
}


//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(permessions_are_saved_correctly, FakeTableOfContents) {
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

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(detects_files_correctly, FakeTableOfContents) {
    boost::filesystem::path test_root("/tmp");
    boost::filesystem::path test_path("/tmp/test_path");
    boost::filesystem::path test_dir("/tmp/test_path/test_dir");
    boost::filesystem::path test_file("/tmp/test_path/test_dir/test_file");
    boost::filesystem::path test_absolute_link("/tmp/test_path/test_absolute_link");
    boost::filesystem::path test_relative_link("/tmp/test_path/test_relative_link");
    boost::filesystem::path test_absolute_dir_link("/tmp/test_path/test_absolute_dir_link");
    boost::filesystem::path test_relative_dir_link("/tmp/test_path/test_relative_dir_link");
    boost::filesystem::path test_absolute_link_to_out("/tmp/test_path/test_absolute_out_link");
    boost::filesystem::remove_all(test_path);

    boost::filesystem::create_directory(test_path);
    boost::filesystem::create_directory(test_dir);
    std::ofstream f(test_file.c_str(), std::ios::ate);
    f << "aaa";
    f.close();
    boost::filesystem::create_symlink(test_file, test_absolute_link);
    boost::filesystem::create_symlink(boost::filesystem::relative(test_file, test_root), test_relative_link);
    boost::filesystem::create_symlink(test_dir, test_absolute_dir_link);
    boost::filesystem::create_symlink(boost::filesystem::relative(test_dir, test_root), test_relative_dir_link);
    boost::filesystem::create_symlink( "/bin/bash", test_absolute_link_to_out);

    PartsCompressionParameters params;
    TableOfContents toc(test_path, params);

    std::shared_ptr<BaseEntry> entry = toc.find(test_dir.lexically_relative(test_root));
    BOOST_REQUIRE(entry);
    std::shared_ptr<DirectoryEntry> dir = std::dynamic_pointer_cast<DirectoryEntry>(entry);
    BOOST_REQUIRE(dir);
    BOOST_CHECK_EQUAL(dir->file(), test_dir.lexically_relative(test_root));
    BOOST_CHECK_EQUAL(dir->owner(), TableOfContents::DEFAULT_OWNER);
    BOOST_CHECK_EQUAL(dir->ownerId(), 0);

    BOOST_CHECK_EQUAL(dir->group(), TableOfContents::DEFAULT_GROUP);
    BOOST_CHECK_EQUAL(dir->groupId(), 0);

    entry = toc.find(test_file.lexically_relative(test_root));
    BOOST_REQUIRE(entry);
    std::shared_ptr<RegularFileEntry> file = std::dynamic_pointer_cast<RegularFileEntry>(entry);
    BOOST_REQUIRE(file);
    // Since currently it is not compressed
    BOOST_CHECK(!file->uncompressedHash().isValid());
    BOOST_CHECK_EQUAL(file->uncompressedSize(), 0);
    BOOST_CHECK_EQUAL(file->compressedSize(), 0);

    entry = toc.find(test_absolute_link.lexically_relative(test_root));
    BOOST_REQUIRE(entry);
    std::shared_ptr<LinkEntry> link = std::dynamic_pointer_cast<LinkEntry>(entry);
    BOOST_REQUIRE(link);

    BOOST_CHECK_EQUAL(link->destination(), test_file.lexically_relative(test_root));
    BOOST_CHECK(link->absolute());

    entry = toc.find(test_relative_dir_link.lexically_relative(test_root));
    BOOST_REQUIRE(entry);
    link = std::dynamic_pointer_cast<LinkEntry>(entry);
    BOOST_REQUIRE(link);

    BOOST_CHECK_EQUAL(link->destination(), test_dir.lexically_relative(test_root));
    BOOST_CHECK(link->absolute() == false);

    entry = toc.find(test_absolute_link_to_out.lexically_relative(test_root));
    BOOST_REQUIRE(entry);
    link = std::dynamic_pointer_cast<LinkEntry>(entry);
    BOOST_REQUIRE(link);

    BOOST_CHECK_EQUAL(link->destination(), "/bin/bash");
    BOOST_CHECK(link->absolute());
}

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(can_unpack_names, FakeTableOfContents) {
    InputBuffer buffer = {0, 3,
                                  0, 3, 'X', 'X', 'X',
                                  0, 2, 'Y', 'Y',
                                  0, 4, 'Z', 'Z', 'z', 'z'};

    std::vector<std::string> names;

    unpackNames(buffer, names);

    BOOST_REQUIRE_EQUAL(names.size(), 3);
    BOOST_CHECK_EQUAL(buffer.size(), 0);

    BOOST_CHECK_EQUAL(names[0], "XXX");
    BOOST_CHECK_EQUAL(names[1], "YY");
    BOOST_CHECK_EQUAL(names[2], "ZZzz");
}
