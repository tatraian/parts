#define BOOST_TEST_MODULE partscreator_test
#include <boost/test/auto_unit_test.hpp>

#include <fakeit/boost/fakeit.hpp>

#include "../partscreator.h"

using namespace fakeit;
using namespace parts;

class FakePartsCreator : public PartsCreator {
public:
    FakePartsCreator() : PartsCreator(boost::filesystem::path("nowhere"),
                                      boost::filesystem::path("somewhere.parts"),
                                      CompressionType::None,
                                      CompressionType::LZMA,
                                      HashType::MD5)
    {}
};


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(can_create_partscreator) {
    PartsCreator pc(boost::filesystem::path("nowhere"),
                    boost::filesystem::path("somewhere.parts"),
                    CompressionType::None,
                    CompressionType::LZMA,
                    HashType::MD5);

}


//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(create_throw_exception_if_root_doesn_t_exist, FakePartsCreator) {
    BOOST_REQUIRE_EXCEPTION(create(), PartsException,
                            [](const std::exception& e){return e.what() == std::string("Path doesn't exist: nowhere");});
}

//==========================================================================================================================================
BOOST_FIXTURE_TEST_CASE(create_throw_exception_if_destination_exists, FakePartsCreator) {
    m_root = boost::filesystem::path("/usr");
    // surely exists on linux...
    m_archive = boost::filesystem::path("/usr");
    BOOST_REQUIRE_EXCEPTION(create(), PartsException,
                            [](const std::exception& e){
                                return e.what() == std::string("Destination archive already exists: /usr");
                            });
}

