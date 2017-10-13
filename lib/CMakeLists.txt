set(parts_SRCS
    partscreator.cpp
    parts.cpp
    filewritebackend.cpp
    contentwritebackend.h
    hash.cpp
    compressor.h
    plaincompressor.cpp
    lzmacompressor.cpp
    tableofcontents.cpp
    baseentry.cpp
    packager.cpp)

add_library(parts ${parts_SRCS})
target_link_libraries(parts Boost::system Boost::filesystem PicoSHA256 lzma::lzma)

if(BUILD_TESTS)
    add_executable(parts_lib_tests
        tests/partscreator_test.cpp
        tests/filewritebackend_test.cpp
        tests/hash_test.cpp
        tests/plaincompressor_test.cpp
        tests/lzmacompressor_test.cpp
        tests/baseentry_test.cpp
        tests/packager_test.cpp
        tests/stackvector_test.cpp)
    target_link_libraries(parts_lib_tests parts FakeIt::FakeIt Boost::test_exec_monitor c++)
    add_test(parts_lib_tester parts_lib_tests)
endif()
