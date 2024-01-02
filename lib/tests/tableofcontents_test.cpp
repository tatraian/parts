#include <gtest/gtest.h>

#include "../tableofcontents.h"
#include "../parts_definitions.h"
#include "../regularfileentry.h"
#include "../linkentry.h"
#include "../directoryentry.h"

#include <filesystem>

#include <fstream>

using namespace parts;

namespace {
PartsCompressionParameters static_parameters;
}

class FakeTableOfContents : public TableOfContents, public ::testing::Test {
public:
    FakeTableOfContents() : TableOfContents(static_parameters)
    {}
};

//==========================================================================================================================================
TEST(tos_tests, TableOfContents_can_parse_only_one_file) {
    const char* filename = "/tmp/one_file";
    PartsCompressionParameters params;

    std::filesystem::remove(filename);
    std::ofstream file(filename, std::ios::ate);

    ASSERT_TRUE(std::filesystem::is_regular_file(filename));

    TableOfContents toc(filename, params);

    ASSERT_TRUE(toc);
    ASSERT_EQ(toc.size(), 1);
    ASSERT_EQ(toc.begin()->first, std::filesystem::path(filename).filename());
    std::shared_ptr<BaseEntry> entry = toc.begin()->second;
    std::shared_ptr<RegularFileEntry> file_entry = std::dynamic_pointer_cast<RegularFileEntry>(entry);
    ASSERT_TRUE(file_entry);
    ASSERT_EQ(file_entry->file(), std::filesystem::path(filename).filename());

    ASSERT_EQ(file_entry->ownerId(), 0);
    ASSERT_EQ(file_entry->owner(), TableOfContents::DEFAULT_OWNER);
    ASSERT_EQ(file_entry->groupId(), 0);
    ASSERT_EQ(file_entry->group(), TableOfContents::DEFAULT_GROUP);
}

//==========================================================================================================================================
TEST_F(FakeTableOfContents, detects_files_correctly) {
    std::filesystem::path test_root("/tmp");
    std::filesystem::path test_path("/tmp/test_path");
    std::filesystem::path test_dir("/tmp/test_path/test_dir");
    std::filesystem::path test_file("/tmp/test_path/test_file");
    std::filesystem::path test_dir_file("/tmp/test_path/test_dir/test_file");
    std::filesystem::path test_absolute_link("/tmp/test_path/test_absolute_link");
    std::filesystem::path test_relative_link("/tmp/test_path/test_dir/test_relative_link");
    std::filesystem::path test_absolute_dir_link("/tmp/test_path/test_absolute_dir_link");
    std::filesystem::path test_relative_dir_link("/tmp/test_path/test_relative_dir_link");
    std::filesystem::path test_absolute_link_to_out("/tmp/test_path/test_absolute_out_link");
    std::filesystem::remove_all(test_path);

    std::filesystem::create_directory(test_path);
    std::filesystem::create_directory(test_dir);
    std::ofstream f(test_file.c_str(), std::ios::ate);
    f << "aaa";
    f.close();
    std::ofstream f2(test_dir_file.c_str(), std::ios::ate);
    f << "bbb";
    f.close();

    std::filesystem::create_symlink(test_dir_file, test_absolute_link);
    std::filesystem::create_symlink(std::filesystem::relative(test_file, test_relative_link.parent_path()), test_relative_link);
    std::filesystem::create_symlink(test_dir, test_absolute_dir_link);
    std::filesystem::create_symlink(std::filesystem::relative(test_dir, test_relative_dir_link.parent_path()), test_relative_dir_link);
    std::filesystem::create_symlink( "/bin/bash", test_absolute_link_to_out);

    PartsCompressionParameters params;
    TableOfContents toc(test_path, params);

    ASSERT_TRUE(toc);
    std::shared_ptr<BaseEntry> entry = toc.find(test_dir.lexically_relative(test_root));
    ASSERT_TRUE(entry);
    std::shared_ptr<DirectoryEntry> dir = std::dynamic_pointer_cast<DirectoryEntry>(entry);
    ASSERT_TRUE(dir);
    ASSERT_EQ(dir->file(), test_dir.lexically_relative(test_root));
    ASSERT_EQ(dir->owner(), TableOfContents::DEFAULT_OWNER);
    ASSERT_EQ(dir->ownerId(), 0);

    ASSERT_EQ(dir->group(), TableOfContents::DEFAULT_GROUP);
    ASSERT_EQ(dir->groupId(), 0);

    entry = toc.find(test_file.lexically_relative(test_root));
    ASSERT_TRUE(entry);
    std::shared_ptr<RegularFileEntry> file = std::dynamic_pointer_cast<RegularFileEntry>(entry);
    ASSERT_TRUE(file);
    ASSERT_EQ(file->uncompressedHash().hashString(), "9834876dcfb05cb167a5c24953eba58c4ac89b1adf57f28f2f9d09af107ee8f0");
    ASSERT_EQ(file->uncompressedSize(), 3);
    // Since currently it is not compressed
    ASSERT_EQ(file->compressedSize(), 0);

    entry = toc.find(test_absolute_link.lexically_relative(test_root));
    ASSERT_TRUE(entry);
    std::shared_ptr<LinkEntry> link = std::dynamic_pointer_cast<LinkEntry>(entry);
    ASSERT_TRUE(link);

    ASSERT_EQ(link->destination(), test_dir_file);

    entry = toc.find(test_relative_dir_link.lexically_relative(test_root));
    ASSERT_TRUE(entry);
    link = std::dynamic_pointer_cast<LinkEntry>(entry);
    ASSERT_TRUE(link);

    ASSERT_EQ(link->destination(), test_dir.filename());

    entry = toc.find(test_absolute_link_to_out.lexically_relative(test_root));
    ASSERT_TRUE(entry);
    link = std::dynamic_pointer_cast<LinkEntry>(entry);
    ASSERT_TRUE(link);

    ASSERT_EQ(link->destination(), "/bin/bash");
}

//==========================================================================================================================================
TEST_F(FakeTableOfContents, can_unpack_names) {
    InputBuffer buffer = {0, 3,
                          0, 3, 'X', 'X', 'X',
                          0, 2, 'Y', 'Y',
                          0, 4, 'Z', 'Z', 'z', 'z'};

    std::vector<std::string> names;

    unpackNames(buffer, names);

    ASSERT_EQ(names.size(), 3);
    ASSERT_EQ(buffer.size(), 0);

    ASSERT_EQ(names[0], "XXX");
    ASSERT_EQ(names[1], "YY");
    ASSERT_EQ(names[2], "ZZzz");
}
