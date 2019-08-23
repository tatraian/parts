#pragma once

#include <string>
#include "contentreadbackend.h"

namespace parts {


class HttpReaderBackend : public ContentReadBackend
{
public:
    HttpReaderBackend(const std::string& file_url);
    ~HttpReaderBackend() noexcept;

    std::string source() const override { return m_fileUrl; }
    void read(std::vector<uint8_t> &data) override;
    void read(InputBuffer& data, size_t size) override;
    void read(uint8_t *data, size_t size) override;
    void seek(const uint64_t &position) override;
    uint64_t position() override { return m_currentPos; }

private:
    std::string m_fileUrl;
    void *m_curlHandle;
    uint64_t m_currentPos = 0;
};

}
