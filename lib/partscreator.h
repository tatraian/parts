#ifndef YARCREATOR_H
#define YARCREATOR_H

#include "parts_definitions.h"

#include <boost/filesystem/path.hpp>
namespace parts
{

class PartsCreator {
public:
    PartsCreator(const boost::filesystem::path& root,
                 const boost::filesystem::path& archive,
                 CompressionType header_compression,
                 CompressionType file_compression,
                 HashType hash_function);

    void create();
protected:
    boost::filesystem::path m_root;
    boost::filesystem::path m_archive;

    CompressionType m_headerCompression;
    CompressionType m_fileCompression;
    HashType m_hashFunction;
};


}


#endif // YARCREATOR_H
