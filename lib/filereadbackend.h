#ifndef FILEREADBACKEND_H
#define FILEREADBACKEND_H

#include "contentreadbackend.h"
#include <boost/filesystem/path.hpp>

#include <fstream>

namespace parts
{

class FileReadBackend : public ContentReadBackend
{
public:
    FileReadBackend(const boost::filesystem::path& filename);
    ~FileReadBackend() override = default;

     std::string source() const override
     { return m_path.string(); }

     void read(uint8_t& data) override;
     void read(uint16_t& data) override;
     void read(uint32_t& data) override;
     void read(uint64_t& data) override;

     void read(std::vector<uint8_t>& data) override;

     void read(uint8_t* data, size_t size) override;

     void seek(const uint64_t& position) override;

protected:
     boost::filesystem::path m_path;
     std::ifstream m_file;
     uint64_t m_filesize;
};

}
#endif // FILEREADBACKEND_H