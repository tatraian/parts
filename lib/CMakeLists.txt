set(parts_SRCS partscreator.cpp parts.cpp filewritebackend.cpp contentwritebackend.h hash.h hash.cpp compressor.h)

add_library(parts ${parts_SRCS})
target_link_libraries(parts Boost::system Boost::filesystem PicoSHA256)

if(BUILD_TESTS)
    add_executable(parts_lib_tests tests/partscreator_test.cpp tests/filewritebackend_test.cpp tests/hash_test.cpp)
    target_link_libraries(parts_lib_tests parts FakeIt::FakeIt Boost::test_exec_monitor c++)
    add_test(parts_lib_tester parts_lib_tests)
endif()
