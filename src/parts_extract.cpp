#include <iostream>
#include <boost/filesystem.hpp>
#include <partsarchive.h>
#ifdef BUILD_HTTP_FEATURES
#include <httpreaderbackend.h>
#endif
#include <filereadbackend.h>
#include <consolelogger.h>
#include <args/args.hxx>

using namespace parts;
using std::string;

//==========================================================================================================================================
std::string cut_slash(std::string orig) {
    while(!orig.empty() && orig.back() == '/')
        orig.pop_back();

    return orig;
}

//==========================================================================================================================================
int main(int argc, char** argv)
{
    ConsoleLogger logger(LOG_LEVELS::INFO, true);
    set_logger(&logger);

    args::ArgumentParser parser("This program extracts or updates files from a \".parts\" archive");
    args::HelpFlag help(parser, "help", "Display help", {'h', "help"});
    args::Positional<std::string> archive_param(parser, "archive", "Path to archive", args::Options::Required);
    args::Positional<std::string> dest_root_dir(parser, "dest_dir", "Path, where to extract", ".");
    args::ValueFlag<std::string> updated_from(parser,
                                              "updated_from",
                                              "The 'old' version, that will be used to update",
                                              {'u', "--updated_from"});
    args::Flag list_only(parser, "list_only", "Only list archive", {'l', "list_only"});

    try
    {
        parser.ParseCLI(argc, argv);

        std::unique_ptr<ContentReadBackend> input_stream;

        if(archive_param.Get().substr(0, 7) == "http://") {
#ifdef BUILD_HTTP_FEATURES
            input_stream.reset(new HttpReaderBackend(archive_param.Get()));
#else
            throw args::ParseError("Http support is missing!");
#endif
        } else {
            input_stream.reset(new FileReadBackend(cut_slash(archive_param.Get())));
        }

        boost::filesystem::path old_dir;
        if (updated_from)
             old_dir= cut_slash(cut_slash(updated_from.Get()));

        boost::filesystem::path dest_dir = cut_slash(dest_root_dir.Get());

        PartsArchive archive(std::move(input_stream));

        if(!boost::filesystem::exists(dest_dir))
        {
            LOG_INFO("Directory {} does not exists, creating it", dest_dir.string());
            boost::filesystem::create_directory(dest_dir);
        }

        if (list_only) {
            throw PartsException("Not implemented!");
        } else if (!updated_from) {
            archive.extractArchive(dest_dir);
        } else {
            archive.updateArchive(old_dir,dest_dir);
        }

        //LOG_INFO("Requests: {}; Bytes received {}", input_stream->sentRequests(), input_stream->readBytes());

        return 0;
    } catch (const args::Help&) {
        std::cout << parser;
        return 0;
    }  catch (const args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
    } catch (const args::RequiredError& e) {
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
        std::cerr << parser;
    }
    return 1;

}
