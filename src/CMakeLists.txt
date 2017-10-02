add_executable(lzma_example lzma_example.cpp)
target_link_libraries(lzma_example lzma::lzma)

add_executable(lzma_decompression lzma_decompression.cpp)
target_link_libraries(lzma_decompression lzma::lzma)
