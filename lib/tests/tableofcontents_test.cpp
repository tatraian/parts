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
BOOST_FIXTURE_TEST_CASE(detects_files_correctly, FakeTableOfContents) {
    boost::filesystem::path test_root("/tmp");
    boost::filesystem::path test_path("/tmp/test_path");
    boost::filesystem::path test_dir("/tmp/test_path/test_dir");
    boost::filesystem::path test_file("/tmp/test_path/test_file");
    boost::filesystem::path test_dir_file("/tmp/test_path/test_dir/test_file");
    boost::filesystem::path test_absolute_link("/tmp/test_path/test_absolute_link");
    boost::filesystem::path test_relative_link("/tmp/test_path/test_dir/test_relative_link");
    boost::filesystem::path test_absolute_dir_link("/tmp/test_path/test_absolute_dir_link");
    boost::filesystem::path test_relative_dir_link("/tmp/test_path/test_relative_dir_link");
    boost::filesystem::path test_absolute_link_to_out("/tmp/test_path/test_absolute_out_link");
    boost::filesystem::remove_all(test_path);

    boost::filesystem::create_directory(test_path);
    boost::filesystem::create_directory(test_dir);
    std::ofstream f(test_file.c_str(), std::ios::ate);
    f << "aaa";
    f.close();
    std::ofstream f2(test_dir_file.c_str(), std::ios::ate);
    f << "bbb";
    f.close();

    boost::filesystem::create_symlink(test_dir_file, test_absolute_link);
    boost::filesystem::create_symlink(boost::filesystem::relative(test_file, test_relative_link.parent_path()), test_relative_link);
    boost::filesystem::create_symlink(test_dir, test_absolute_dir_link);
    boost::filesystem::create_symlink(boost::filesystem::relative(test_dir, test_relative_dir_link.parent_path()), test_relative_dir_link);
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
    BOOST_CHECK_EQUAL(file->uncompressedHash().hashString(), "9834876dcfb05cb167a5c24953eba58c4ac89b1adf57f28f2f9d09af107ee8f0");
    BOOST_CHECK_EQUAL(file->uncompressedSize(), 3);
    // Since currently it is not compressed
    BOOST_CHECK_EQUAL(file->compressedSize(), 0);

    entry = toc.find(test_absolute_link.lexically_relative(test_root));
    BOOST_REQUIRE(entry);
    std::shared_ptr<LinkEntry> link = std::dynamic_pointer_cast<LinkEntry>(entry);
    BOOST_REQUIRE(link);

    BOOST_CHECK_EQUAL(link->destination(), test_dir_file);

    entry = toc.find(test_relative_dir_link.lexically_relative(test_root));
    BOOST_REQUIRE(entry);
    link = std::dynamic_pointer_cast<LinkEntry>(entry);
    BOOST_REQUIRE(link);

    BOOST_CHECK_EQUAL(link->destination(), test_dir.leaf());

    entry = toc.find(test_absolute_link_to_out.lexically_relative(test_root));
    BOOST_REQUIRE(entry);
    link = std::dynamic_pointer_cast<LinkEntry>(entry);
    BOOST_REQUIRE(link);

    BOOST_CHECK_EQUAL(link->destination(), "/bin/bash");
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
