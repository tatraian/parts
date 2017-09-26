#ifndef FILEWRITEBACKEND_H
#define FILEWRITEBACKEND_H

#include "contentwritebackend.h"

#include <fstream>

#include <boost/filesystem/path.hpp>

namespace parts
{

class FileWriteBackend : public ContentWriteBackend
{
public:
    FileWriteBackend(const boost::filesystem::path& file);
    ~FileWriteBackend() override;

    void append(uint8_t value) override;
    void append(uint16_t value) override;
    void append(uint32_t value) override;
    void append(uint64_t value) override;
    void append(const std::vector<uint8_t>& value) override;

    uint64_t getPosition() override;
    void concatenate(ContentWriteBackend&& tail) override;

protected:
    void resetToRead() override;
    void read(std::vector<uint8_t>&buffer) override;

    void checkFile() const;

protected:
    boost::filesystem::path m_filename;
    std::fstream m_file;
};

}
#endif // FILEWRITEBACKEND_H
