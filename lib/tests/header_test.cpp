#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include "../contentreadbackend.h"
#include "../header.h"

using namespace parts;
using namespace fakeit;

//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(Header_can_be_read_from_backend) {

    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(std::vector<uint8_t>&))).Do([](std::vector<uint8_t>& a){ a = {'p','a','r','t','s','!'}; });

    When(OverloadedMethod(backend, read, void(uint8_t&))).
            // version
            Do([](uint8_t& a) {a = 1;}).
            // toc compression type
            Do([](uint8_t& a) {a = static_cast<uint8_t>(CompressionType::LZMA);}).
            // file compressino type
            Do([](uint8_t& a) {a = static_cast<uint8_t>(CompressionType::LZMA);}).
            // hash type
            Do([](uint8_t& a) {a = static_cast<uint8_t>(HashType::SHA256);}).
            // dummys
            Do([](uint8_t& a) {a = 0;}).
            Do([](uint8_t& a) {a = 0;});


    When(OverloadedMethod(backend, read, void(uint32_t&))).Do([](uint32_t& a){ a = 4567; });
    When(Method(backend, source)).AlwaysReturn("Test");

    std::unique_ptr<Header> header;
    BOOST_REQUIRE_NO_THROW(header.reset(new Header(backend.get())));

    BOOST_CHECK(header->getHashType() == HashType::SHA256);
    BOOST_CHECK(header->getTocCompressionType() == CompressionType::LZMA);
    BOOST_CHECK(header->getFileCompressionType() == CompressionType::LZMA);

    BOOST_CHECK_EQUAL(header->getTocSize(), 4567);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(Header_throws_exception_in_case_of_bad_magic) {
    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(std::vector<uint8_t>&))).Do([](std::vector<uint8_t>& a){ a = {'x','x','x','x','x','x'}; });
    When(Method(backend, source)).AlwaysReturn("Test");

    std::unique_ptr<Header> header;
    BOOST_REQUIRE_THROW(header.reset(new Header(backend.get())), PartsException);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(Header_throws_exception_if_bad_version) {
    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(std::vector<uint8_t>&))).Do([](std::vector<uint8_t>& a){ a = {'p','a','r','t','s','!'}; });
    When(Method(backend, source)).AlwaysReturn("Test");

    When(OverloadedMethod(backend, read, void(uint8_t&))).Do([](uint8_t& a) {a = 2;});

    std::unique_ptr<Header> header;
    BOOST_REQUIRE_THROW(header.reset(new Header(backend.get())), PartsException);
}

