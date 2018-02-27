#include <iostream>
#include <boost/filesystem/path.hpp>

#include <partsarchive.h>
#include <filereadbackend.h>
#include <consolelogger.h>

using namespace parts;

int main() {
    ConsoleLogger logger(LOG_LEVELS::DEBUG, true);
    set_logger(&logger);
    std::unique_ptr<FileReadBackend> input_stream(new FileReadBackend(boost::filesystem::path("/tmp/archive.parts")));
    PartsArchive archive(std::move(input_stream));

    archive.extractArchive("/tmp/test_extract");
}
