cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE YES)

option(BUILD_TESTS "Build tests too" ON)
option(BUILD_CMDLINE "Build command line programs too" ON)
option(BUILD_HTTP "Build http input reader too" OFF)
option(SKIP_CONAN "Skip conan thirdparty handling" OFF)

if (NOT SKIP_CONAN)
    set(CONAN_PROFILE ${CMAKE_SOURCE_DIR}/thirdparty/conan/profiles/clang14)
    set(CMAKE_PROJECT_INCLUDE_BEFORE ${CMAKE_SOURCE_DIR}/cmake/conan_thirdparties.cmake)
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/thirdparties/conan)
endif()

project(Parts VERSION 1.0.0)

set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO "RelWithDebInfo;Release")
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG "Debug;RelWithDebInfo;Release")

set(targets_export_name "${PROJECT_NAME}Targets")

find_package(LibLZMA CONFIG REQUIRED)
find_package(ZLIB CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
# if (BUILD_HTTP)
#     find_package(CURL CONFIG REQUIRED)
# endif()

if (BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)
    find_package(FakeIt CONFIG REQUIRED)
endif()

add_definitions(-Wall -Werror=switch-enum)

include_directories(SYSTEM thirdparty)
add_subdirectory(lib)
include_directories(lib)
add_subdirectory(src)
#add_subdirectory(scripts)


# set(config_export_name "${PROJECT_NAME}Config")
# set(project_config "${CMAKE_CURRENT_BINARY_DIR}/${config_export_name}.cmake")
# set(version_config "${CMAKE_CURRENT_BINARY_DIR}/${config_export_name}Version.cmake")
# set(config_install_dir "lib/cmake/${PROJECT_NAME}")
# set(namespace "${PROJECT_NAME}::")

# include(CMakePackageConfigHelpers)
# configure_package_config_file( # Uses target_exports_name
#     "cmake/config.cmake.in"
#     "${project_config}"
#     INSTALL_DESTINATION "${config_install_dir}"
# )
# write_basic_package_version_file(
# "${version_config}" COMPATIBILITY SameMajorVersion)

# install(
#     EXPORT "${targets_export_name}"
#     NAMESPACE "${namespace}"
#     DESTINATION "${config_install_dir}")

# install(
#     FILES "${project_config}" "${version_config}"
#     DESTINATION "${config_install_dir}"
# )

# include(InstallRequiredSystemLibraries)

# set(CPACK_GENERATOR RPM)
# set(CPACK_PACKAGE_NAME "Parts")
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Archiving utility for embedded systems")
# set(CPACK_PACKAGE_DESCRIPTION "\".parts\" files hold the table of contents at the beggining of the file. So downloading and extracting "
#     "similar content (with small binary different) can be done only with the binary diff downloading.")
# set(CPACK_PACKAGE_VERSION ${Parts_VERSION})
# set(CPACK_RPM_PACKAGE_LICENSE "MIT")
# set(CPACK_STRIP_FILES TRUE)

# include(CPack)
