#include <partsarchive.h>
#include <filereadbackend.h>
#include <consolelogger.h>

#include <iostream>

using namespace parts;

int main(int argc, char** argv) {
    ConsoleLogger logger(LOG_LEVELS::INFO);
    set_logger(&logger);

    if (argc != 2){
        std::cerr << "No previous version directory" << std::endl;
        return 1;
    }

    std::unique_ptr<FileReadBackend> input_stream(new FileReadBackend(boost::filesystem::path("/tmp/archive.parts")));
    PartsArchive archive(std::move(input_stream));

    archive.updateArchive(argv[1], "/tmp");

    return 0;
}

