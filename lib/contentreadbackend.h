#ifndef CONTENTREADBACKEND_H
#define CONTENTREADBACKEND_H

#include <vector>
#include <string>

namespace parts {

class ContentReadBackend {
public:
    virtual ~ContentReadBackend() {}

    virtual std::string source() const = 0;

    virtual void read(uint8_t& data) = 0;
    virtual void read(uint16_t& data) = 0;
    virtual void read(uint32_t& data) = 0;
    virtual void read(uint64_t& data) = 0;
    // will read data.size() bytes
    virtual void read(std::vector<uint8_t>& data) = 0;

    virtual void seek(const uint64_t& position) = 0;
};

}

#endif // CONTENTREADBACKEND_H
