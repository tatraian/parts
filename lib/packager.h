#ifndef PACKAGER_H
#define PACKAGER_H

#include "parts_definitions.h"

#include <string>
#include <deque>

#include <boost/filesystem/path.hpp>

namespace parts
{

class Packager
{
public:
    static void append(std::vector<uint8_t>& output, uint8_t value);
    static void append(std::vector<uint8_t>& output, uint16_t value);
    static void append(std::vector<uint8_t>& output, uint32_t value);
    static void append(std::vector<uint8_t>& output, uint64_t value);
    static void append(std::vector<uint8_t>& output, const std::vector<uint8_t>& value);
    static void append(std::vector<uint8_t>& output, const boost::filesystem::path& value);
    template<class SizeType>
    static void append(std::vector<uint8_t>& output, const std::string& value){
        SizeType size = static_cast<SizeType>(value.size());
        if (std::numeric_limits<SizeType>::max() < value.size())
            throw PartsException("String cannot be stored on: " + std::string(typeid(SizeType).name()));
        append(output, size);
        output.insert(output.end(),
                      reinterpret_cast<const uint8_t*>(&value[0]),
                      reinterpret_cast<const uint8_t*>(&value[0]) + size);
    }
};

}
#endif // PACKAGER_H
