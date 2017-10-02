#include <lzma.h>

#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: lzma_decompress input_file output_file\n";
        return 3;
    }

    lzma_stream lzma_context = LZMA_STREAM_INIT;

    lzma_ret result = lzma_stream_decoder(&lzma_context, UINT64_MAX, LZMA_CONCATENATED);
    if (result != LZMA_OK) {
        std::cerr << "Cannot create decoder, stopping\n";
        return 1;
    }

    std::ifstream input_file(argv[1]);
    std::ofstream output_file(argv[2]);

    if (!input_file) {
        std::cerr << "Cannot open file: " << argv[1] << std::endl;
        return 3;
    }

    if (!output_file) {
        std::cerr << "Cannot open file: " << argv[2] << std::endl;
        return 3;
    }

    uint8_t ibuf[1024*1024];
    uint8_t obuf[1024*1024];

    lzma_context.next_in = nullptr;
    lzma_context.avail_in = 0;
    lzma_context.next_out = obuf;
    lzma_context.avail_out = 1024*1024;
    lzma_action action = LZMA_RUN;

    for(;;){
        if (lzma_context.avail_in == 0 && !input_file.eof()) {
            input_file.read(reinterpret_cast<char*>(ibuf), 1024*1024);
            lzma_context.next_in = ibuf;
            lzma_context.avail_in = input_file.gcount();

            if (input_file.eof()) {
                action = LZMA_FINISH;
                std::cerr << "end of file reached\n";
            }
        }

        result = lzma_code(&lzma_context, action);

        if (lzma_context.avail_out == 0 || result == LZMA_STREAM_END) {
            size_t bytes_to_write = 1024*1024 - lzma_context.avail_out;

            output_file.write(reinterpret_cast<char*>(obuf), bytes_to_write);
            lzma_context.next_out = obuf;
            lzma_context.avail_out = 1024*1024;
        }

        if (result != LZMA_OK)
            break;
    }

    lzma_end(&lzma_context);
    return 0;
}

