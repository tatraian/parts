#include <lzma.h>

#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char** argv){
    int main_return = 0;
    if (argc < 3) {
        std::cerr << "Usage: lzma_example input_file output_file\n";
        return 3;
    }
    ifstream input_file(argv[1], ios::in | ios::binary);
    ofstream output_file(argv[2], ios::out | ios::binary);

    if (!input_file) {
        std::cerr << "Cannot open file: " << argv[1] << std::endl;
        return 3;
    }

    if (!output_file) {
        std::cerr << "Cannot open file: " << argv[2] << std::endl;
        return 3;
    }

    char ibuffer[1 * 1024 * 1024];
    char obuffer[1 * 1024 * 1024];

    lzma_stream lzma_context = LZMA_STREAM_INIT;
    lzma_ret ret = lzma_easy_encoder(&lzma_context, 9, LZMA_CHECK_CRC64);
    if (ret != LZMA_OK)
    {
        std::cerr << "no way to continue...\n";
        return 1;
    }

    lzma_context.next_in = nullptr;
    lzma_context.avail_in = 0;
    lzma_context.next_out = reinterpret_cast<uint8_t*>(obuffer);
    lzma_context.avail_out = 1024*1024;
    lzma_action action = LZMA_RUN;

    for(;;) {

        if (lzma_context.avail_in == 0 && !input_file.eof()) {
            std::cerr << "reading from file: ";
            lzma_context.next_in = reinterpret_cast<uint8_t*>(ibuffer);
            input_file.read(ibuffer, 1024*1024);
            lzma_context.avail_in = input_file.gcount();
            std::cerr << input_file.gcount() << std::endl;

            if (input_file.eof()) {
                action = LZMA_FINISH;
                std::cerr << "end of file reached\n";
            }
        }

        lzma_ret result = lzma_code(&lzma_context, action);
        std::cerr << "result: " << result << std::endl;

        if (lzma_context.avail_out == 0 || result == LZMA_STREAM_END) {
            size_t bytes_to_write = 1024*1024 - lzma_context.avail_out;
            std::cerr << "writing to file: " << bytes_to_write << std::endl;
            output_file.write(obuffer, bytes_to_write);
            lzma_context.next_out = reinterpret_cast<uint8_t*>(obuffer);
            lzma_context.avail_out = 1024*1024;
        }

        if (result == LZMA_STREAM_END)
            break;

        if (result != LZMA_OK){
            std::cerr << "lzma error\n";
            main_return = 2;
        }

    }

    lzma_end(&lzma_context);
    return main_return;
}
