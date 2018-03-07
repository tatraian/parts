if (BUILD_CMDLINE)
    if (BUILD_HTTP)
        add_definitions( -DBUILD_HTTP_FEATURES=1 )
    endif()

    add_executable(parts_compress parts_compress.cpp)
    target_link_libraries(parts_compress parts)

    add_executable(parts_extract parts_extract.cpp)
    target_link_libraries(parts_extract parts)

    install(TARGETS parts_compress parts_extract
            EXPORT ${targets_export_name}
            RUNTIME DESTINATION "bin")
endif()
