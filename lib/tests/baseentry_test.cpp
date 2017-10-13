#include <boost/test/auto_unit_test.hpp>

#include "../baseentry.h"

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

    void compressEntry(Compressor& compressor, ContentWriteBackend& backend) override {};
};
}

/*
//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_pack_base_data) {
    TestBaseEntry entry("file1", 0644, "PARTS_DEFAULT", 2, "PARTS_DEFAULT", 1);

    std::vector<uint8_t> result = entry.getRaw();

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
*/
