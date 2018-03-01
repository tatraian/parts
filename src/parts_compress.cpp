#include <partsarchive.h>
#include <consolelogger.h>

#include <iostream>
#include <lzma.h>

int main(int argc, char** argv) {
    parts::ConsoleLogger logger(parts::LOG_LEVELS::DEBUG);
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
    parameters.m_lzmaParameters.m_compressionLevel = (9 | LZMA_PRESET_EXTREME);
    parameters.m_lzmaParameters.m_x86FilterActive = true;
    parameters.m_hashType = parts::HashType::MD5;
    parts::PartsArchive archive(root_name, parameters);

    archive.createArchive("/tmp/archive.parts");
    return 0;
}

