#ifndef CONTENTREADBACKEND_H
#define CONTENTREADBACKEND_H

#include "inputbuffer.h"

#include <vector>
#include <string>

namespace parts {

class ContentReadBackend {
public:
    ContentReadBackend() : m_readBytes(0), m_sentRequests(0) {}
    virtual ~ContentReadBackend() = default;

    ContentReadBackend(const ContentReadBackend&) = delete;
    ContentReadBackend& operator=(const ContentReadBackend&) = delete;

    ContentReadBackend(ContentReadBackend&&) = default;
    ContentReadBackend& operator=(ContentReadBackend&&) = default;

    virtual std::string source() const = 0;

    // will read data.size() bytes
    virtual void read(std::vector<uint8_t>& data) = 0;

    virtual void read(InputBuffer& data, size_t size) = 0;

    virtual void read(uint8_t* data, size_t size) = 0;

    virtual void seek(const uint64_t& position) = 0;

    virtual uint64_t position() = 0;

    uint64_t readBytes() const
    { return m_readBytes; }

    uint64_t sentRequests() const
    { return m_sentRequests; }

protected:
    uint64_t m_readBytes;
    uint64_t m_sentRequests;
};

}

#endif // CONTENTREADBACKEND_H
