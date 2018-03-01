#include <iostream>
#include <boost/filesystem/path.hpp>

#include <partsarchive.h>
#include <networkfilereaderbackend.h>
#include <consolelogger.h>

using namespace parts;

int main(int argc, const char** argv) {
    if (argc != 2)
    {
        std::cerr << "Usage: parts_web_extract <url>" << std::endl;
        return 1;
    }

    ConsoleLogger logger(LOG_LEVELS::DEBUG, true);
    set_logger(&logger);
    std::unique_ptr<NetworkFileReaderBackend> input_stream(new NetworkFileReaderBackend(argv[1]));
    PartsArchive archive(std::move(input_stream));
    archive.extractArchive("/tmp/test_extract");
}
