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

     void read(std::vector<uint8_t>& data) override;

     void read(InputBuffer& data, size_t size) override;

     void read(uint8_t* data, size_t size) override;

     void seek(const uint64_t& position) override;

     uint64_t position() override;

     uint64_t readBytes() const override
     { return 0; }

     uint64_t sentRequests() const override
     { return 0; }

protected:
     boost::filesystem::path m_path;
     std::ifstream m_file;
     uint64_t m_filesize;
};

}
#endif // FILEREADBACKEND_H
