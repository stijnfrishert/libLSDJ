#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>

#include "../liblsdj/sav.h"

int handle_error(lsdj_error_t* error)
{
    std::cerr << "ERROR: " << lsdj_get_error_c_str(error) << std::endl;
    lsdj_free_error(error);
    return 1;
}

void exportProject(const lsdj_project_t* project, boost::filesystem::path folder, bool addVersionNumber, bool putInFolder, lsdj_error_t** error)
{
    char name[9];
    lsdj_project_get_name(project, name, sizeof(name));
    
    if (putInFolder)
        folder /= name;
    boost::filesystem::create_directories(folder);
    
    std::stringstream stream;
    stream << name;
    if (addVersionNumber)
        stream << "." << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)lsdj_project_get_version(project);
    stream << ".lsdsng";
    folder /= stream.str();
    
    lsdj_write_lsdsng_to_file(project, folder.c_str(), error);
}

int exportSongs(const std::string& file, bool addVersionNumber, bool putInFolder)
{
    lsdj_error_t* error = nullptr;
    lsdj_sav_t* sav = lsdj_read_sav_from_file(boost::filesystem::canonical(file).c_str(), &error);
    if (sav == nullptr)
    {
        lsdj_free_sav(sav);
        return handle_error(error);
    }
    
    const auto& cwd = boost::filesystem::current_path();
    
    const auto count = lsdj_sav_get_project_count(sav);
    for (int i = 0; i < count; ++i)
    {
        lsdj_project_t* project = lsdj_sav_get_project(sav, i);
        lsdj_song_t* song = lsdj_project_get_song(project);
        if (!song)
            continue;
        
        exportProject(project, cwd, addVersionNumber, putInFolder, &error);
        if (error)
            return handle_error(error);
    }
    
    lsdj_free_sav(sav);
    
    return 0;
}

int main(int argc, char* argv[])
{
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help screen")
        ("file", boost::program_options::value<std::string>(), ".sav file, can be a nameless option")
        ("noversion,n", "Don't add version numbers to the filename")
        ("folder,f", "Put every lsdsng in its own folder");
    
    boost::program_options::positional_options_description positionalOptions;
    positionalOptions.add("file", 1);
    
    try
    {
        boost::program_options::variables_map vm;
        boost::program_options::command_line_parser parser(argc, argv);
        parser = parser.options(desc);
        parser = parser.positional(positionalOptions);
        boost::program_options::store(parser.run(), vm);
        boost::program_options::notify(vm);
    
        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        } else if (vm.count("file")) {
            return exportSongs(vm["file"].as<std::string>(), !vm.count("noversion"), vm.count("folder"));
        } else {
            std::cout << desc << std::endl;
            return 0;
        }
    } catch (const boost::program_options::error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

	return 0;
}
