#include <partsarchive.h>
#include <consolelogger.h>

#include <iostream>

int main(int argc, char** argv) {
    parts::ConsoleLogger logger(parts::LOG_LEVELS::TRACE);
    parts::set_logger(&logger);
    if (argc < 2) {
        std::cerr << "Please give a file!\n";
        return 1;
    }

    std::string root_name = argv[1];
    while (!root_name.empty() &&
           root_name.back() == '/') {
        root_name.pop_back();
    }

    parts::PartsCompressionParameters parameters;
    parts::PartsArchive archive(root_name, parameters);

    archive.createArchive("/tmp/archive.parts");
    return 0;
}

