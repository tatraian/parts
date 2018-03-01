add_executable(lzma_example lzma_example.cpp)
target_link_libraries(lzma_example lzma::lzma)

add_executable(lzma_decompression lzma_decompression.cpp)
target_link_libraries(lzma_decompression lzma::lzma)

add_executable(parts_compress parts_compress.cpp)
target_link_libraries(parts_compress parts)

add_executable(parts_list parts_list.cpp)
target_link_libraries(parts_list parts)

add_executable(parts_extract parts_extract.cpp)
target_link_libraries(parts_extract parts)

add_executable(parts_web_extract parts_web_extract.cpp)
target_link_libraries(parts_web_extract parts)

add_executable(parts_update parts_update.cpp)
target_link_libraries(parts_update parts)
