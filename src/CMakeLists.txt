add_executable(lzma_example lzma_example.cpp)
target_link_libraries(lzma_example lzma::lzma)

add_executable(lzma_decompression lzma_decompression.cpp)
target_link_libraries(lzma_decompression lzma::lzma)

add_executable(parts_compress parts_compress.cpp)
target_link_libraries(parts_compress parts)

