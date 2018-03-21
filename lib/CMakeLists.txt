if (BUILD_HTTP)
    set(HTTP_SOURCES
        httpreaderbackend.h
        httpreaderbackend.cpp)
    set(HTTP_LINK CURL::libcurl)
endif()


set(parts_SRCS
    logger.h
    logger.cpp
    filewritebackend.cpp
    contentwritebackend.h
    hash.cpp
    compressor.h
    plaincompressor.cpp
    lzmacompressor.cpp
    zlibcompressor.cpp
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
    plaindecompressor.h
    plaindecompressor.cpp
    zlibdecompressor.h
    zlibdecompressor.cpp
    compressorfactory.h
    compressorfactory.cpp
    logger_internal.h
    internal_definitions.h
    ${HTTP_SOURCES}
    partsjobinterface.h
    partsupdatejob.h
    partsupdatejob.cpp
    )

add_library(parts ${parts_SRCS})
target_link_libraries(parts PUBLIC Boost::system Boost::filesystem)
target_link_libraries(parts PRIVATE lzma::lzma ZLIB::zlib fmt::fmt ${HTTP_LINK})

install(TARGETS parts
    EXPORT ${targets_export_name}
    INCLUDES DESTINATION include
    RUNTIME DESTINATION  bin
    LIBRARY DESTINATION  lib
    ARCHIVE DESTINATION  lib)

install(DIRECTORY "."
    DESTINATION "include/parts"
    FILES_MATCHING PATTERN "*.h"
    PATTERN "tests" EXCLUDE
    PATTERN "logger_internal.h" EXCLUDE
    PATTERN "internal_definitions.h" EXCLUDE
    )

if(BUILD_TESTS)
    add_executable(parts_lib_tests
        tests/filewritebackend_test.cpp
        tests/hash_test.cpp
        tests/plaincompressor_test.cpp
        tests/lzmacompressor_test.cpp
        tests/zlibcompressor_test.cpp
        tests/baseentry_test.cpp
        tests/regularfileentry_test.cpp
        tests/directoryentry_test.cpp
        tests/linkentry_test.cpp
        tests/packager_test.cpp
        tests/tableofcontents_test.cpp
        tests/header_test.cpp
        tests/lzmadecompressor_test.cpp
        tests/zlibdecompressor_test.cpp
        tests/filereadbackend_test.cpp)
    target_link_libraries(parts_lib_tests parts FakeIt::FakeIt Boost::test_exec_monitor c++)
    add_test(parts_lib_tester parts_lib_tests)
endif()
