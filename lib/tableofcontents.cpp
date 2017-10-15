#include "tableofcontents.h"

#include <boost/filesystem.hpp>

using namespace parts;

const std::string DEFAULT_OWNER = "__PARTS_DEFAULT_OWNER__";
const std::string DEFAULT_GROUP = "__PARTS_DEFAULT_GROUP__";


//==========================================================================================================================================
TableOfContents::TableOfContents(const boost::filesystem::path& root, const PartsCompressionParameters& parameters) :
    m_parameters(parameters)
{
    m_owners.push_back(DEFAULT_OWNER);
    m_groups.push_back(DEFAULT_GROUP);

    if (!boost::filesystem::exists(root))
        throw PartsException("Root directory doesn't exist: " + root.string());

    if (boost::filesystem::is_regular_file(root))
    {

    }
}
