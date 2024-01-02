function(print_conan_version)
    execute_process(
        COMMAND conan --version
        OUTPUT_VARIABLE __output
        COMMAND_ERROR_IS_FATAL ANY
    )
message("${__output}")
endfunction()

function(get_conan_packages _install_dir)
    execute_process(
        COMMAND
            conan install ${CMAKE_SOURCE_DIR}/thirdparty/conan/conanfile.py
                          --output-folder ${_install_dir}
                          --profile:build=${CONAN_PROFILE}
                          --profile:host=${CONAN_PROFILE}
                          --build=missing
                          --settings=build_type=Release
    )
endfunction()

print_conan_version()
get_conan_packages(${CMAKE_BINARY_DIR}/thirdparties/conan)
