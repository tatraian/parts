set(parts_SRCS
    logger.h
    logger.cpp
    filewritebackend.cpp
    contentwritebackend.h
    hash.cpp
    compressor.h
    plaincompressor.cpp
    lzmacompressor.cpp
    tableofcontents.cpp
    baseentry.cpp
    regularfileentry.cpp
    directoryentry.cpp
    linkentry.cpp
    packager.cpp
    partsarchive.cpp
    header.cpp
    contentreadbackend.h
    decompressor.h
    decompressorfactory.cpp
    lzmadecompressor.cpp
    simpleguard.h
    filereadbackend.cpp
    consolelogger.h
    consolelogger.cpp
    inputbuffer.h
    )

add_library(parts ${parts_SRCS})
target_link_libraries(parts Boost::system Boost::filesystem PicoSHA256 lzma::lzma fmt::fmt)
target_include_directories(parts INTERFACE .)

if(BUILD_TESTS)
    add_executable(parts_lib_tests
        tests/filewritebackend_test.cpp
        tests/hash_test.cpp
        tests/plaincompressor_test.cpp
        tests/lzmacompressor_test.cpp
        tests/baseentry_test.cpp
        tests/regularfileentry_test.cpp
        tests/directoryentry_test.cpp
        tests/linkentry_test.cpp
        tests/packager_test.cpp
        tests/tableofcontents_test.cpp
        tests/header_test.cpp
        tests/lzmadecompressor_test.cpp
        tests/filereadbackend_test.cpp)
    target_link_libraries(parts_lib_tests parts FakeIt::FakeIt Boost::test_exec_monitor c++)
    add_test(parts_lib_tester parts_lib_tests)
endif()
