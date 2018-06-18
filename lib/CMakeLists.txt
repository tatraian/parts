if (BUILD_HTTP)
    set(HTTP_SOURCES
        httpreaderbackend.h
        httpreaderbackend.cpp)
    set(HTTP_LINK CURL::libcurl)
endif()


set(parts_SRCS
    # logging
    logger_internal.h
    logger.h
    logger.cpp
    consolelogger.h
    consolelogger.cpp


    # generic infrastructure
    hash.h
    hash.cpp
    inputbuffer.h
    internal_definitions.h
    packager.h
    packager.cpp
    simpleguard.h

    # write backends
    contentwritebackend.h
    filewritebackend.h
    filewritebackend.cpp

    # reader backends
    contentreadbackend.h
    filereadbackend.h
    filereadbackend.cpp
    ${HTTP_SOURCES}

    # compressors
    compressor.h
    compressorfactory.h
    compressorfactory.cpp
    plaincompressor.h
    plaincompressor.cpp
    lzmacompressor.h
    lzmacompressor.cpp
    zlibcompressor.h
    zlibcompressor.cpp

    #decompressors
    decompressor.h
    decompressorfactory.h
    decompressorfactory.cpp
    plaindecompressor.h
    plaindecompressor.cpp
    lzmadecompressor.h
    lzmadecompressor.cpp
    zlibdecompressor.h
    zlibdecompressor.cpp

    # archive generic
    partsarchive.h
    partsarchive.cpp
    header.h
    header.cpp
    tableofcontents.h
    tableofcontents.cpp

    baseentry.h
    baseentry.cpp
    directoryentry.h
    directoryentry.cpp
    linkentry.h
    linkentry.cpp
    regularfileentry.h
    regularfileentry.cpp

    # job interface
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
        tests/baseentry_test.cpp
        tests/directoryentry_test.cpp
        tests/filereadbackend_test.cpp
        tests/filewritebackend_test.cpp
        tests/hash_test.cpp
        tests/header_test.cpp
        tests/linkentry_test.cpp
        tests/lzmacompressor_test.cpp
        tests/lzmadecompressor_test.cpp
        tests/packager_test.cpp
        tests/plaincompressor_test.cpp
        tests/regularfileentry_test.cpp
        tests/tableofcontents_test.cpp
        tests/zlibcompressor_test.cpp
        tests/zlibcompressor_test.cpp
        tests/zlibdecompressor_test.cpp)
    target_link_libraries(parts_lib_tests parts FakeIt::FakeIt Boost::test_exec_monitor c++)
    add_test(parts_lib_tester parts_lib_tests)
endif()
