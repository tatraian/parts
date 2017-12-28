#include <partsarchive.h>

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Please give a file!\n";
        return 1;
    }

    parts::PartsCompressionParameters parameters;
    parts::PartsArchive archive(argv[1], parameters);

    archive.createArchive("/tmp/archive.parts");
    return 0;
}

