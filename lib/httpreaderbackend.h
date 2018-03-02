#pragma once

#include <string>
#include "contentreadbackend.h"

namespace parts {


class HttpReaderBackend : public ContentReadBackend
{
private:
    std::string m_FileUrl;
    void *m_CurlHandle;
    uint64_t m_CurrentPos = 0;
public:
    HttpReaderBackend(const std::string& file_url);
    ~HttpReaderBackend();

    struct ReadStatistic
    {
        uint64_t* m_BytesRead = nullptr;
        uint64_t* m_RequestsDone = nullptr;
        void AddReadCount(uint64_t c) const { if(m_BytesRead) *m_BytesRead += c; }
        void RequestDone() const { if(m_RequestsDone) ++*m_RequestsDone; }
    };
    ReadStatistic m_ReadStat;
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
