#include <gmock/gmock.h>
#include <fakeit.hpp>

#include "../contentreadbackend.h"
#include "../header.h"

using namespace parts;
using namespace fakeit;


//==========================================================================================================================================
TEST(header, Header_can_be_read_from_backend) {

    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(InputBuffer&, size_t))).Do([](InputBuffer& b, size_t s){
        if (s != 16) {
            // Header has 16 bytes
            ASSERT_TRUE(false);
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
    ASSERT_NO_THROW(header.reset(new Header(backend.get())));
    ASSERT_TRUE(*header);

    ASSERT_TRUE(header->getHashType() == HashType::SHA256);
    ASSERT_TRUE(header->getTocCompressionType() == CompressionType::LZMA);

    ASSERT_EQ(header->getTocSize(), 4567);
}


//==========================================================================================================================================
TEST(header, Header_will_be_invalid_in_case_of_bad_magic) {
    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(InputBuffer&, size_t))).Do([](InputBuffer& b, size_t s){
        InputBuffer tmp = {'x','x','x','x','x','x', 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};
        b = tmp;
    });
    When(Method(backend, source)).AlwaysReturn("Test");

    Header header(backend.get());
    ASSERT_TRUE(!header);
}


//==========================================================================================================================================
TEST(header, Header_will_be_invalid_if_bad_version) {
    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(InputBuffer&, size_t))).Do([](InputBuffer& b, size_t s){
        InputBuffer tmp = {'p','a','r','t','s','!', 1, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};
        b = tmp;
    });
    When(Method(backend, source)).AlwaysReturn("Test");

    Header header(backend.get());
    ASSERT_TRUE(!header);
}


//==========================================================================================================================================
TEST(header, Header_will_be_invalid_there_is_no_enough_data) {
    Mock<ContentReadBackend> backend;
    When(OverloadedMethod(backend, read, void(InputBuffer&, size_t))).Do([](InputBuffer& b, size_t s){
        InputBuffer tmp = {'p','a','r','t','s','!', 1, 0 ,0 ,0 ,0 ,0};
        b = tmp;
    });
    When(Method(backend, source)).AlwaysReturn("Test");

    Header header(backend.get());
    ASSERT_TRUE(!header);
}
