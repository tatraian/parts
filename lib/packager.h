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
    static void pop_front(std::deque<uint8_t>& input, uint8_t& value);
    static void pop_front(std::deque<uint8_t>& input, uint16_t& value);
    static void pop_front(std::deque<uint8_t>& input, uint32_t& value);
    static void pop_front(std::deque<uint8_t>& input, uint64_t& value);
    static void pop_front(std::deque<uint8_t>& input, std::vector<uint8_t>& value);
    static void pop_front(std::deque<uint8_t>& input, boost::filesystem::path& value);
    template<class SizeType>
    static void pop_front(std::deque<uint8_t>& input, std::string& value){
        SizeType size;
        pop_front(input, size);
        if (input.size() < size)
            throw PartsException("No enough data to read value");
        value.clear();
        value.insert(0, reinterpret_cast<const char*>(&input.front()), size);
        input.erase(input.begin(), input.begin() + size);
    }

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
