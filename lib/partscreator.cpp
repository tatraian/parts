#include "partscreator.h"
#include "contentwritebackend.h"

#include <boost/filesystem.hpp>

using namespace parts;


//==========================================================================================================================================
PartsCreator::PartsCreator(const boost::filesystem::path& root,
                           const boost::filesystem::path& archive,
                           CompressionType header_compression,
                           CompressionType file_compression,
                           HashType hash_function) :
    m_root(root),
    m_archive(archive),
    m_headerCompression(header_compression),
    m_fileCompression(file_compression),
    m_hashFunction(hash_function)
{

}


//==========================================================================================================================================
void PartsCreator::create()
{
    if (!boost::filesystem::exists(m_root))
        throw PartsException("Path doesn't exist: " + m_root.string());

    if (boost::filesystem::exists(m_archive))
        throw PartsException("Destination archive already exists: " + m_archive.string());


}
