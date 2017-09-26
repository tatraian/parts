#ifndef PARTS_H
#define PARTS_H

#include "parts_definitions.h"

#include <boost/filesystem.hpp>

namespace parts
{

void create_archive(const boost::filesystem::path& root,
                    const boost::filesystem::path& archive,
                    CompressionType header_compression,
                    CompressionType file_compression,
                    HashType hash_function);

}



#endif // PARTS_H
