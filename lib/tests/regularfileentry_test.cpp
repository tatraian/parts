#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include <fstream>

#include "../regularfileentry.h"
#include "../compressor.h"
#include "../internal_definitions.h"

using namespace parts;
using namespace fakeit;

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(compress_fills_missing_entries_and_after_it_is_packed_correctly) {
    // Sorry, since it is much to create data entry, I put the two tests in one case...
    boost::filesystem::path path("/tmp/file1");
    std::vector<uint8_t> input(4, 0);
    for(size_t tmp = 0; tmp != input.size(); ++tmp)
        input[tmp] = tmp;

    std::ofstream file(path.string(), std::ios::binary | std::ios::ate);
    file.write(reinterpret_cast<char*>(&input[0]), input.size());
    file.close();

    Hash hash(HashType::SHA256, input);
    PartsCompressionParameters parameters;
    parameters.m_fileCompression = CompressionType::None;
    parameters.m_tocCompression = CompressionType::None;
    parameters.m_hashType = HashType::SHA256;
    RegularFileEntry entry(path, path, 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1, parameters.m_fileCompression, parameters, 0, input.size(), 0);

    std::vector<uint8_t> compressed;
    Mock<ContentWriteBackend> backend_mock;
    When(Method(backend_mock, getPosition)).Return(100);
    When(OverloadedMethod(backend_mock, append, void(const std::vector<uint8_t>&))).AlwaysDo([&](const std::vector<uint8_t>& a){compressed = a;});

    /** YES, we compress the same file */
    entry.compressEntry("", backend_mock.get());

    Verify(Method(backend_mock, getPosition)).Once();

    BOOST_REQUIRE_EQUAL_COLLECTIONS(input.begin(), input.end(), compressed.begin(), compressed.end());

    BOOST_REQUIRE_EQUAL(entry.uncompressedSize(), 4);
    BOOST_REQUIRE_EQUAL(entry.compressedSize(), 4);
    BOOST_REQUIRE_EQUAL(entry.offset(), 100);

    std::vector<uint8_t> result;

    entry.append(result);

    BOOST_REQUIRE_EQUAL(result.size(), 76);
    BOOST_REQUIRE_EQUAL(result[0], static_cast<uint8_t>(EntryTypes::RegularFile));

    // base entry check
    BOOST_REQUIRE_EQUAL(result[1], 0u);
    BOOST_REQUIRE_EQUAL(result[2], 10u);

    BOOST_REQUIRE_EQUAL(result[3], '/');
    BOOST_REQUIRE_EQUAL(result[4], 't');
    BOOST_REQUIRE_EQUAL(result[5], 'm');
    BOOST_REQUIRE_EQUAL(result[6], 'p');
    BOOST_REQUIRE_EQUAL(result[7], '/');
    BOOST_REQUIRE_EQUAL(result[8], 'f');
    BOOST_REQUIRE_EQUAL(result[9], 'i');
    BOOST_REQUIRE_EQUAL(result[10], 'l');
    BOOST_REQUIRE_EQUAL(result[11], 'e');
    BOOST_REQUIRE_EQUAL(result[12], '1');

    BOOST_REQUIRE_EQUAL(result[13], 1u);
    BOOST_REQUIRE_EQUAL(result[14], 0244);

    BOOST_REQUIRE_EQUAL(result[15], 0u);
    BOOST_REQUIRE_EQUAL(result[16], 2u);

    BOOST_REQUIRE_EQUAL(result[17], 0u);
    BOOST_REQUIRE_EQUAL(result[18], 1u);

    // Regular file entry
    // Hash
    BOOST_REQUIRE_EQUAL_COLLECTIONS(result.begin() + 19, result.begin() + 19 + 32, hash.hash().begin(), hash.hash().end());
    // uncompressed size
    BOOST_REQUIRE_EQUAL(result[51], 0);
    BOOST_REQUIRE_EQUAL(result[52], 0);
    BOOST_REQUIRE_EQUAL(result[53], 0);
    BOOST_REQUIRE_EQUAL(result[54], 0);
    BOOST_REQUIRE_EQUAL(result[55], 0);
    BOOST_REQUIRE_EQUAL(result[56], 0);
    BOOST_REQUIRE_EQUAL(result[57], 0);
    BOOST_REQUIRE_EQUAL(result[58], 4);

    // compression type
    BOOST_REQUIRE_EQUAL(result[59], 0);

    // compressed size
    BOOST_REQUIRE_EQUAL(result[60], 0);
    BOOST_REQUIRE_EQUAL(result[61], 0);
    BOOST_REQUIRE_EQUAL(result[62], 0);
    BOOST_REQUIRE_EQUAL(result[63], 0);
    BOOST_REQUIRE_EQUAL(result[64], 0);
    BOOST_REQUIRE_EQUAL(result[65], 0);
    BOOST_REQUIRE_EQUAL(result[66], 0);
    BOOST_REQUIRE_EQUAL(result[67], 4);

    // offset
    BOOST_REQUIRE_EQUAL(result[68], 0);
    BOOST_REQUIRE_EQUAL(result[69], 0);
    BOOST_REQUIRE_EQUAL(result[70], 0);
    BOOST_REQUIRE_EQUAL(result[71], 0);
    BOOST_REQUIRE_EQUAL(result[72], 0);
    BOOST_REQUIRE_EQUAL(result[73], 0);
    BOOST_REQUIRE_EQUAL(result[74], 0);
    BOOST_REQUIRE_EQUAL(result[75], 100);
}

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(regular_file_entry_can_be_created_from_input_stream) {
    InputBuffer input = {0, 5, 'f', 'i', 'l', 'e', '1', 1, 0244, 0, 0, 0, 0,
                                 // hash
                                 0x88, 0xd4, 0x26, 0x6f, 0xd4, 0xe6, 0x33, 0x8d,
                                 0x13, 0xb8, 0x45, 0xfc, 0xf2, 0x89, 0x57, 0x9d,
                                 0x20, 0x9c, 0x89, 0x78, 0x23, 0xb9, 0x21, 0x7d,
                                 0xa3, 0xe1, 0x61, 0x93, 0x6f, 0x03, 0x15, 0x89,
                                 // uncompressed size
                                 0, 0, 0, 0, 0, 0, 0, 100,
                                 // compression type
                                 1,
                                 // compressed size
                                 0, 0, 0, 0, 0, 0, 0, 42,
                                 // offset
                                 0, 0, 0, 0, 0, 0, 1, 0
                                 };

    std::vector<std::string> owners = {"DEFAULT_OWNER"};
    std::vector<std::string> groups = {"DEFAULT_GROUP"};

    PartsCompressionParameters parameters;
    parameters.m_fileCompression = CompressionType::None;
    parameters.m_tocCompression = CompressionType::None;
    parameters.m_hashType = HashType::SHA256;
    RegularFileEntry entry(input, owners, groups, parameters);

    BOOST_REQUIRE_EQUAL(input.size(), 0);
    //base entries
    BOOST_CHECK_EQUAL(entry.file(), "file1");
    BOOST_CHECK_EQUAL(entry.permissions(), 0644);
    BOOST_CHECK_EQUAL(entry.owner(), "DEFAULT_OWNER");
    BOOST_CHECK_EQUAL(entry.group(), "DEFAULT_GROUP");
    BOOST_CHECK_EQUAL(entry.ownerId(), 0);
    BOOST_CHECK_EQUAL(entry.groupId(), 0);

    BOOST_CHECK_EQUAL(entry.uncompressedHash().hashString(), "88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
    BOOST_CHECK_EQUAL(entry.uncompressedSize(), 100u);
    BOOST_CHECK_EQUAL(entry.compressedSize(), 42u);
    BOOST_CHECK_EQUAL(entry.offset(), 256u);
}
