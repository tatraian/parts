#pragma once

#include <string>
#include "contentreadbackend.h"

namespace parts {


class NetworkFileReaderBackend : public ContentReadBackend
{
private:
    std::string m_FileUrl;
    void *m_CurlHandle;
    uint64_t m_CurrentPos = 0;
public:
    NetworkFileReaderBackend(const std::string& file_url);
    ~NetworkFileReaderBackend();


    // ContentReadBackend interface
public:
    std::string source() const override { return m_FileUrl; }
    void read(uint8_t &data) override;
    void read(uint16_t &data) override;
    void read(uint32_t &data) override;
    void read(uint64_t &data) override;
    void read(std::vector<uint8_t> &data) override;
    void read(uint8_t *data, size_t size) override;
    void seek(const uint64_t &position) override;
    uint64_t position() override { return m_CurrentPos; }
};

}
