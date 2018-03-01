cmake_minimum_required(VERSION 3.3)
set(CMAKE_CXX_STANDARD 14)

include("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.108.tar.gz"
    SHA1 "56cf9f1d52296373dbcc0d0c9c2b5526a0815019"
)

option(BUILD_TESTS "Build tests too" ON)

project(Parts)

hunter_add_package(Boost COMPONENTS system filesystem)
find_package(Boost CONFIG REQUIRED system filesystem)
hunter_add_package(lzma)
find_package(lzma)
hunter_add_package(fmt)
find_package(fmt CONFIG REQUIRED)
hunter_add_package(CURL)
find_package(CURL CONFIG REQUIRED)

if (BUILD_TESTS)
    hunter_add_package(Boost COMPONENTS test)
    find_package(Boost CONFIG REQUIRED test_exec_monitor)
    hunter_add_package(FakeIt)
    find_package(FakeIt)
endif()

add_definitions(-Wall -Werror=switch-enum)

add_subdirectory(thirdparty)
add_subdirectory(lib)
add_subdirectory(src)

