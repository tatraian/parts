#include <iostream>
#include <iomanip>
#include <boost/filesystem/path.hpp>

#include <partsarchive.h>
#include <filereadbackend.h>

using namespace parts;

int main() {
    std::unique_ptr<FileReadBackend> input_stream(new FileReadBackend(boost::filesystem::path("/tmp/archive.parts")));
    PartsArchive archive(std::move(input_stream));

    std::cout << "Number of entries: " << archive.toc().size() << std::endl;
    for(const auto& entry : archive.toc()) {
        std::cout << entry.first << " --- " << entry.second->owner() << ":" << entry.second->group()
                  << " (" << std::oct << entry.second->permissions() << ")" << std::endl;
    }
}
