cmake_minimum_required(VERSION 3.3)
set(CMAKE_CXX_STANDARD 14)

include("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.108.tar.gz"
    SHA1 "56cf9f1d52296373dbcc0d0c9c2b5526a0815019"
)

option(BUILD_TESTS "Build tests too" ON)
option(BUILD_CMDLINE "Build command line programs too" ON)
option(BUILD_HTTP "Build http input reader too" ON)

project(Parts VERSION 1.0.0)

set(targets_export_name "${PROJECT_NAME}Targets")

hunter_add_package(Boost COMPONENTS system filesystem)
find_package(Boost CONFIG REQUIRED system filesystem)
hunter_add_package(lzma)
find_package(lzma)
hunter_add_package(fmt)
find_package(fmt CONFIG REQUIRED)
if (BUILD_HTTP)
    hunter_add_package(CURL)
    find_package(CURL CONFIG REQUIRED)
endif()

if (BUILD_TESTS)
    hunter_add_package(Boost COMPONENTS test)
    find_package(Boost CONFIG REQUIRED test_exec_monitor)
    hunter_add_package(FakeIt)
    find_package(FakeIt)
endif()

add_definitions(-Wall -Werror=switch-enum)

include_directories(thirdparty)
add_subdirectory(lib)
include_directories(lib)
add_subdirectory(src)
add_subdirectory(scripts)


set(config_export_name "${PROJECT_NAME}Config")
set(project_config "${CMAKE_CURRENT_BINARY_DIR}/${config_export_name}.cmake")
set(version_config "${CMAKE_CURRENT_BINARY_DIR}/${config_export_name}Version.cmake")
set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)
configure_package_config_file( # Uses target_exports_name
    "cmake/config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)
write_basic_package_version_file(
"${version_config}" COMPATIBILITY SameMajorVersion)

install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}")

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR RPM)
set(CPACK_PACKAGE_NAME "Parts")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Archiving utility for embedded systems")
set(CPACK_PACKAGE_DESCRIPTION "\".parts\" files hold the table of contents at the beggining of the file. So downloading and extracting "
    "similar content (with small binary different) can be done only with the binary diff downloading.")
set(CPACK_PACKAGE_VERSION ${Parts_VERSION})
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_STRIP_FILES TRUE)

include(CPack)
