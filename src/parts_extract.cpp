#include <iostream>
#include <boost/filesystem.hpp>
#include <partsarchive.h>
#ifdef BUILD_HTTP_FEATURES
#include <httpreaderbackend.h>
#endif
#include <filereadbackend.h>
#include <consolelogger.h>
#include <cxxopts.hpp>

using namespace parts;
using std::string;

string usage()\
{
#ifdef BUILD_HTTP_FEATURES
    return "parts_web_extract (-u url|-f file) [-o old_dir]";
#else
    return "parts_web_extract -f file [-o old_dir]";
#endif
}



int main(int argc, char** argv)
{
    cxxopts::Options options("MyProgram", "One line description of MyProgram");
    options.add_options()
            ("h,help", "Print usage")
            ("f,file", "parst file to extract from", cxxopts::value<std::string>())
        #ifdef BUILD_HTTP_FEATURES
            ("u,url", "url to extract from", cxxopts::value<std::string>())
        #endif
            ("o,old_dir", "previous version extracted directory", cxxopts::value<std::string>())
            ("t,target_dir", "extract dir", cxxopts::value<std::string>())
      ;
    try
    {
        options.parse(argc,argv);
    }
    catch(const cxxopts::OptionException& ex)
    {
        std::cerr << "Fauled to parse arguments: " << ex.what() <<  std::endl;
        std::cerr << "Usage: " << usage() << std::endl;
        return 1;
    }
    if (options["h"].as<bool>())
    {
        std::cout << usage() << std::endl;
        return 0;
    }
#ifdef BUILD_HTTP_FEATURES
    boost::filesystem::path url = options["u"].as<string>();
    boost::filesystem::path file = options["f"].as<string>();
    if (url.empty() == file.empty())
    {
        std::cerr << "Please specify exactly one of --url or --file" << std::endl;
        return 1;
    }
#else
    boost::filesystem::path file = options["f"].as<string>();
    if (file.empty())
    {
        std::cerr << "Please specify --file" << std::endl;
        return 1;
    }
#endif

    boost::filesystem::path target = options["t"].as<string>();
    if (target.empty())
    {
        std::cerr << "Please specify a target" << std::endl;
        return 1;
    }
    boost::filesystem::path old_dir = options["o"].as<string>();


    ConsoleLogger logger(LOG_LEVELS::DEBUG, true);
    set_logger(&logger);
    std::unique_ptr<ContentReadBackend> input_stream;

#ifdef BUILD_HTTP_FEATURES
    uint64_t l_bytesRead = 0;
    uint64_t l_requestsDone = 0;
#endif
    if (file.size())
    {
        LOG_DEBUG("Creating file backend with parameter {}",file.string());
        input_stream.reset(new FileReadBackend(file));
    }
#ifdef BUILD_HTTP_FEATURES
    else
    {
        LOG_DEBUG("Creating http backend with parameter {}",url.string());
        auto backend = new HttpReaderBackend(url.string());
        input_stream.reset(backend);
        backend->m_ReadStat.m_BytesRead = & l_bytesRead;
        backend->m_ReadStat.m_RequestsDone = & l_requestsDone;
    }
#endif
    PartsArchive archive(std::move(input_stream));

    if(!boost::filesystem::exists(target))
    {
        LOG_DEBUG("Directory {} does not exists, creating it",target.string());
        boost::filesystem::create_directory(target);
    }
    if (old_dir.empty())
    {
        archive.extractArchive(target);
    }
    else
    {
        archive.updateArchive(old_dir,target);
    }

#ifdef BUILD_HTTP_FEATURES
    if (url.size())
    {
        LOG_INFO("Http requests done: {}; Bytes received {}", l_requestsDone, l_bytesRead);
    }
#endif
}
