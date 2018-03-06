#include <partsarchive.h>
#include <consolelogger.h>
#include <args/args.hxx>

#include <iostream>
#include <lzma.h>

using namespace parts;

//==========================================================================================================================================
std::string cut_slash(std::string orig) {
    while(!orig.empty() && orig.back() == '/')
        orig.pop_back();

    return orig;
}

//==========================================================================================================================================
struct HashReader
{
    void operator()(const std::string& name, const std::string& value, HashType& hash_type) {
        std::string lower_value = value;
        std::transform(lower_value.begin(), lower_value.end(), lower_value.begin(), ::tolower);

        if (lower_value == "md5") {
            hash_type = HashType::MD5;
        } else if (lower_value == "sha256") {
            hash_type = HashType::SHA256;
        } else {
            throw args::ParseError("Unknown hash type: " + value + " (supported: md5, sha256)");
        }
    }
};

//==========================================================================================================================================
struct LzmaLevelReader
{
    void operator()(const std::string& name, const std::string& value, int& level) {
        try
        {
            level = std::stoi(value);
        } catch(const std::exception&) {
            throw args::ParseError("Invalid lzma level value: " + value);
        }
        if (level < 0 || level > 9)
            throw args::ParseError("lzma level is a number from 1 to 9");
    }
};

//==========================================================================================================================================
int main(int argc, char** argv) {
    ConsoleLogger logger(LOG_LEVELS::TRACE);
    set_logger(&logger);

    args::ArgumentParser parser("This program compresses a directory or a file into '.parts' format.");
    args::HelpFlag help(parser, "help", "Display help", {'h', "help"});
    args::ValueFlag<HashType, HashReader> hash_type(parser, "hash_type", "Hash type (default: md5)", {"hash"}, HashType::MD5);
    args::Flag lzma_extrem(parser, "lzma_extrem", "Extrem flag of LZMA compression", {"lzma_extrem"});
    args::Flag lzma_machine_code(parser, "lzma_machine_code", "Switch on LZMA machine code optimization (for X86)", {"lzma_machine_code"});
    args::ValueFlag<int, LzmaLevelReader> lzma_level(parser, "lzma_level", "Set lzma compression level (1-9)", {"lzma_level"}, 6);
    args::Positional<std::string> archive_name(parser, "archive_name", "The name of the archive", args::Options::Required);
    args::Positional<std::string> compress_dir(parser, "compress_dir", "The file/dir to be compressed", args::Options::Required);

    try
    {
        parser.ParseCLI(argc, argv);
        std::string compress_entry = cut_slash(compress_dir.Get());

        parts::PartsCompressionParameters parameters;
        parameters.m_fileCompression = CompressionType::LZMA;
        parameters.m_tocCompression = CompressionType::LZMA;
        parameters.m_hashType = hash_type.Get();

        parameters.m_lzmaParameters.m_compressionLevel = lzma_level.Get();
        if (lzma_extrem.Get())
            parameters.m_lzmaParameters.m_compressionLevel |= LZMA_PRESET_EXTREME;
        if (lzma_machine_code.Get())
            parameters.m_lzmaParameters.m_x86FilterActive = true;

        std::string archive_filename = archive_name.Get();
        if (archive_filename.size() <= 6 /* .parts */ || archive_filename.substr(archive_filename.size() - 6) != ".parts")
            archive_filename += ".parts";

        parts::PartsArchive archive(compress_entry, parameters);

        archive.createArchive(archive_filename);

        return 0;
    } catch(const args::Help&) {
        std::cout << parser;
    } catch (const args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
    } catch (const args::RequiredError& e) {
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
        std::cerr << parser;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 1;
}

