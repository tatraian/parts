#include "plaindecompressor.h"
#include "parts_definitions.h"
#include "internal_definitions.h"
#include "logger_internal.h"

#include <fstream>


using namespace parts;

//==========================================================================================================================================
InputBuffer PlainDecompressor::extractBuffer(const std::vector<uint8_t>& buffer, size_t decompressed_size)
{
    InputBuffer result;
    result.insert(result.begin(), buffer.begin(), buffer.end());
    return result;
}

//==========================================================================================================================================
void PlainDecompressor::extractFile(const boost::filesystem::path& file,
                                    ContentReadBackend& backend,
                                    size_t position,
                                    size_t compressed_size,
                                    size_t decompressed_size)
{
    std::ofstream output(file.string(), std::ios::binary | std::ios::ate);
    if (!output) {
        throw PartsException ("Cannot create file "+file.string());
    }

    // if the last entry of the file is uncompressed empty file than we want to positioning outside of the archive...
    if (compressed_size == 0)
        return;

    backend.seek(position);
    size_t remained = compressed_size;
    std::vector<uint8_t> buffer(4 * MB, 0);
    while(remained != 0) {
        buffer.resize(std::min(4*MB, remained));
        backend.read(buffer);
        output.write((char*)&buffer[0], buffer.size());
        remained -= buffer.size();
    }
}
