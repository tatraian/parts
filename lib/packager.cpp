#include "packager.h"

#include <boost/endian/conversion.hpp>

using namespace parts;

namespace
{

//==========================================================================================================================================
template<class Value>
void append_internal(std::vector<uint8_t>& output, Value value)
{
    boost::endian::native_to_big_inplace(value);
    output.insert(output.end(), reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + sizeof(value));
}

//==========================================================================================================================================
template<class Value>
void pop_front_internal(InputBuffer& input, Value& value)
{
    if (input.size() < sizeof(value))
        throw PartsException("No enough data to read value");
    value = reinterpret_cast<Value&>(input.front());
    boost::endian::big_to_native_inplace(value);
    input.erase(input.begin(), input.begin() + sizeof(Value));
}

}

//==========================================================================================================================================
void Packager::pop_front(InputBuffer& input, uint8_t& value)
{
    pop_front_internal(input, value);
}

//==========================================================================================================================================
void Packager::pop_front(InputBuffer& input, uint16_t& value)
{
    pop_front_internal(input, value);
}

//==========================================================================================================================================
void Packager::pop_front(InputBuffer& input, uint32_t& value)
{
    pop_front_internal(input, value);
}

//==========================================================================================================================================
void Packager::pop_front(InputBuffer& input, uint64_t& value)
{
    pop_front_internal(input, value);
}

//==========================================================================================================================================
void Packager::pop_front(InputBuffer& input, std::vector<uint8_t>& value)
{
    size_t size = value.size();
    value.clear();

    if (input.size() < size)
        throw PartsException("No enough data to read value");

    value.insert(value.end(), input.begin(), input.begin() + size);
    input.erase(input.begin(), input.begin() + size);
}

//==========================================================================================================================================
void Packager::pop_front(InputBuffer& input, std::filesystem::path& value)
{
    std::string tmp;
    pop_front<uint16_t>(input, tmp);
    value = tmp;
}

//==========================================================================================================================================
void Packager::append(std::vector<uint8_t>& output, uint8_t value)
{
    append_internal(output, value);
}

//==========================================================================================================================================
void Packager::append(std::vector<uint8_t>& output, uint16_t value)
{
    append_internal(output, value);
}

//==========================================================================================================================================
void Packager::append(std::vector<uint8_t>& output, uint32_t value)
{
    append_internal(output, value);
}

//==========================================================================================================================================
void Packager::append(std::vector<uint8_t> &output, uint64_t value)
{
    append_internal(output, value);
}

//==========================================================================================================================================
void Packager::append(std::vector<uint8_t>& output, const std::vector<uint8_t>& value)
{
    output.insert(output.end(), value.begin(), value.end());
}

//==========================================================================================================================================
void Packager::append(std::vector<uint8_t> &output, const std::filesystem::path& value)
{
    append<uint16_t>(output, value.string());
}

