#include "packager.h"

#include <boost/endian/conversion.hpp>

using namespace parts;

namespace
{

//==========================================================================================================================================
template<class Value>
void append_internal(std::deque<uint8_t>& output, Value value)
{
    boost::endian::native_to_big_inplace(value);
    output.insert(output.end(), reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + sizeof(value));
}

}

//==========================================================================================================================================
void Packager::append(std::deque<uint8_t>& output, uint8_t value)
{
    append_internal(output, value);
}

//==========================================================================================================================================
void Packager::append(std::deque<uint8_t>& output, uint16_t value)
{
    append_internal(output, value);
}

//==========================================================================================================================================
void Packager::append(std::deque<uint8_t>& output, uint32_t value)
{
    append_internal(output, value);
}

//==========================================================================================================================================
void Packager::append(std::deque<uint8_t> &output, uint64_t value)
{
    append_internal(output, value);
}

//==========================================================================================================================================
void Packager::append(std::deque<uint8_t>& output, const std::vector<uint8_t>& value)
{
    output.insert(output.end(), value.begin(), value.end());
}

//==========================================================================================================================================
void Packager::append(std::deque<uint8_t> &output, const boost::filesystem::path& value)
{
    append<uint16_t>(output, value.string());
}
