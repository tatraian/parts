#ifndef CONTENTWRITEBACKEND_H
#define CONTENTWRITEBACKEND_H

#include <vector>
#include <cstdint>

namespace parts
{

class ContentWriteBackend
{
public:
    virtual ~ContentWriteBackend() = default;

    virtual void append(uint8_t) = 0;
    virtual void append(uint16_t) = 0;
    virtual void append(uint32_t) = 0;
    virtual void append(uint64_t) = 0;
    virtual void append(const std::vector<uint8_t>&) = 0;

    virtual uint64_t getPosition() = 0;
    virtual void concatenate(ContentWriteBackend&&) = 0;

protected:
    virtual void resetToRead() = 0;
    virtual void read(std::vector<uint8_t>&) = 0;
};


}
#endif // CONTENTWRITEBACKEND_H
