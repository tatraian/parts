#include <iostream>
#include <boost/filesystem/path.hpp>

#include <partsarchive.h>
#include <filereadbackend.h>

using namespace parts;

int main() {
    std::unique_ptr<FileReadBackend> input_stream(new FileReadBackend(boost::filesystem::path("/tmp/archive.parts")));
    PartsArchive archive(std::move(input_stream));

    archive.extractArchive("/tmp/test_extract");
}
