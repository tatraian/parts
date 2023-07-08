#include "plaincompressor.h"

#include <fstream>

#include "parts_definitions.h"
#include "internal_definitions.h"

using namespace parts;

//==========================================================================================================================================
size_t PlainCompressor::compressFile(const std::filesystem::path& path, ContentWriteBackend& backend)
{
    std::ifstream input_file(path.string(), std::ios::binary | std::ios::in);
    if (!input_file)
        throw PartsException("Cannot open file for reading: " + path.string());

    std::vector<uint8_t> input_buffer(MB * 4, 0);
    size_t size = 0;
    for(;;) {
        input_buffer.resize(MB*4);
        input_file.read(reinterpret_cast<char*>(&input_buffer[0]), MB * 4);
        if (input_file.gcount() == 0)
            break;
        size += input_file.gcount();
        input_buffer.resize(size);
        backend.append(input_buffer);
    }

    return size;
}

//==========================================================================================================================================
size_t PlainCompressor::compressBuffer(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& backend)
{
    backend.insert(backend.end(), buffer.begin(), buffer.end());

    return buffer.size();
}
