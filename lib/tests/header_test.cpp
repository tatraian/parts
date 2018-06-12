#include <boost/test/auto_unit_test.hpp>
#include <fakeit/boost/fakeit.hpp>

#include "../contentreadbackend.h"
#include "../header.h"

using namespace parts;
using namespace fakeit;


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(Header_can_be_read_from_backend) {

    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(InputBuffer&, size_t))).Do([](InputBuffer& b, size_t s){
        if (s != 16) {
            // Header has 16 bytes
            BOOST_REQUIRE(false);
        }
        InputBuffer tmp = {'p','a','r','t','s','!',
                           2,
                           static_cast<uint8_t>(CompressionType::LZMA),
                           static_cast<uint8_t>(HashType::SHA256),
                           0,
                           0,
                           0,
                           0, 0, 0x11, 0xd7};
        b = tmp;
    });

    When(Method(backend, source)).AlwaysReturn("Test");

    std::unique_ptr<Header> header;
    BOOST_REQUIRE_NO_THROW(header.reset(new Header(backend.get())));

    BOOST_CHECK(header->getHashType() == HashType::SHA256);
    BOOST_CHECK(header->getTocCompressionType() == CompressionType::LZMA);

    BOOST_CHECK_EQUAL(header->getTocSize(), 4567);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(Header_throws_exception_in_case_of_bad_magic) {
    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(InputBuffer&, size_t))).Do([](InputBuffer& b, size_t s){
        InputBuffer tmp = {'x','x','x','x','x','x', 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};
        b = tmp;
    });
    When(Method(backend, source)).AlwaysReturn("Test");

    std::unique_ptr<Header> header;
    BOOST_REQUIRE_THROW(header.reset(new Header(backend.get())), PartsException);
}


//==========================================================================================================================================
BOOST_AUTO_TEST_CASE(Header_throws_exception_if_bad_version) {
    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(InputBuffer&, size_t))).Do([](InputBuffer& b, size_t s){
        InputBuffer tmp = {'p','a','r','t','s','!', 1, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};
        b = tmp;
    });
    When(Method(backend, source)).AlwaysReturn("Test");

    std::unique_ptr<Header> header;
    BOOST_REQUIRE_THROW(header.reset(new Header(backend.get())), PartsException);
}
