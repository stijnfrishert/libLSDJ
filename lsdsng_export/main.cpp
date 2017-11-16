#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "../liblsdj/sav.h"

enum class VersionStyle
{
    NONE,
    HEX,
    DECIMAL
};

VersionStyle versionStyle;
bool underscore = false;
bool putInFolder = false;
bool verbose = false;
std::vector<int> indices;

int handle_error(lsdj_error_t* error)
{
    std::cerr << "ERROR: " << lsdj_get_error_c_str(error) << std::endl;
    lsdj_free_error(error);
    return 1;
}

std::string constructName(const lsdj_project_t* project, bool underscore)
{
    char name[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    lsdj_project_get_name(project, name, sizeof(name));
    
    if (underscore)
        std::replace(name, name + 9, 'x', '_');
    
    return name;
}

void exportProject(const lsdj_project_t* project, boost::filesystem::path folder, VersionStyle versionStyle, bool underscore, bool putInFolder, lsdj_error_t** error)
{
    const auto name = constructName(project, underscore);
    
    if (putInFolder)
        folder /= name;
    boost::filesystem::create_directories(folder);
    
    std::stringstream stream;
    stream << name;
    switch (versionStyle)
    {
        case VersionStyle::NONE: break;
        case VersionStyle::HEX:
            stream << "." << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)lsdj_project_get_version(project);
            break;
        case VersionStyle::DECIMAL:
            stream << "." << std::setfill('0') << std::setw(3) << (unsigned int)lsdj_project_get_version(project);
            break;
    }
    stream << ".lsdsng";
    folder /= stream.str();
    
    lsdj_write_lsdsng_to_file(project, folder.string().c_str(), error);
}

int exportSongs(const boost::filesystem::path& path, const std::string& output)
{
    lsdj_error_t* error = nullptr;
    lsdj_sav_t* sav = lsdj_read_sav_from_file(path.string().c_str(), &error);
    if (sav == nullptr)
    {
        lsdj_free_sav(sav);
        return handle_error(error);
    }
    
    if (verbose)
        std::cout << "Read '" << path.string() << "'" << std::endl;
    
    const auto outputFolder = boost::filesystem::absolute(output);
    
    const auto count = lsdj_sav_get_project_count(sav);
    for (int i = 0; i < count; ++i)
    {
        if (!indices.empty() && std::find(std::begin(indices), std::end(indices), i + 1) == std::end(indices))
            continue;
        
        lsdj_project_t* project = lsdj_sav_get_project(sav, i);
        lsdj_song_t* song = lsdj_project_get_song(project);
        if (!song)
            continue;
        
        exportProject(project, outputFolder, versionStyle, underscore, putInFolder, &error);
        if (error)
            return handle_error(error);
        
        if (verbose)
        {
            std::cout << "Exported " << constructName(project, underscore);
            
            switch (versionStyle)
            {
                case VersionStyle::NONE: break;
                case VersionStyle::HEX:
                    std::cout << " (" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)lsdj_project_get_version(project) << ")";
                    break;
                case VersionStyle::DECIMAL:
                    std::cout << " (" << std::setfill('0') << std::setw(3) << (unsigned int)lsdj_project_get_version(project) << ")";
                    break;
            }
            
            std::cout << std::endl;
        }
    }
    
    lsdj_free_sav(sav);
    
    return 0;
}

int print(const boost::filesystem::path& path)
{
    lsdj_error_t* error = nullptr;
    lsdj_sav_t* sav = lsdj_read_sav_from_file(path.string().c_str(), &error);
    if (sav == nullptr)
    {
        lsdj_free_sav(sav);
        return handle_error(error);
    }
    
    const auto active = lsdj_sav_get_active_project(sav);

    if (indices.empty())
    {
        std::cout << "WM. ";
        if (active != -1)
        {
            lsdj_project_t* project = lsdj_sav_get_project(sav, active);
            
            const auto name = constructName(project, underscore);
            std::cout << name;
            for (auto i = 0; i < (8 - name.length()); ++i)
                std::cout << ' ';
        } else {
            std::cout << "        ";
        }
        
    
        const lsdj_song_t* song = lsdj_sav_get_song(sav);
        if (lsdj_song_get_file_changed_flag(song))
            std::cout << "\t*";
    
        std::cout << std::endl;
    }
    
    const auto count = lsdj_sav_get_project_count(sav);
    for (int i = 0; i < count; ++i)
    {
        if (!indices.empty() && std::find(std::begin(indices), std::end(indices), i + 1) == std::end(indices))
            continue;
        
        const lsdj_project_t* project = lsdj_sav_get_project(sav, i);
        const lsdj_song_t* song = lsdj_project_get_song(project);
        if (!song)
            continue;
        
        std::cout << std::to_string(i + 1) << ". ";
        if (i < 9)
            std::cout << ' ';
        
        const auto name = constructName(project, underscore);
        std::cout << name;
        
        for (auto i = 0; i < (8 - name.length()); ++i)
            std::cout << ' ';
        
        switch (versionStyle)
        {
            case VersionStyle::NONE: break;
            case VersionStyle::HEX:
                std::cout << '\t'<< std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)lsdj_project_get_version(project);
                break;
            case VersionStyle::DECIMAL:
                std::cout << '\t' << std::setfill('0') << std::setw(3) << (unsigned int)lsdj_project_get_version(project);
                break;
        }
        
        std::cout << std::endl;
    }
    
    return 0;
}

int main(int argc, char* argv[])
{
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help screen")
        ("file", boost::program_options::value<std::string>(), "Input save file, can be a nameless option")
        ("noversion,n", "Don't add version numbers to the filename")
        ("folder,f", "Put every lsdsng in its own folder")
        ("print,p", "Print a list of all songs in the sav")
        ("decimal,d", "Use decimal notation for the version number, instead of hex")
        ("underscore,u", "Use an underscore for the special lightning bolt character, instead of x")
        ("output,o", boost::program_options::value<std::string>()->default_value(""), "Output folder for the lsdsng's")
        ("verbose,v", "Verbose output during export")
        ("index,i", boost::program_options::value<std::vector<int>>(), "Select a given project to export, 0 or more");
    
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
            versionStyle = vm.count("noversion") ? VersionStyle::NONE : vm.count("decimal") ? VersionStyle::DECIMAL : VersionStyle::HEX;
            underscore = vm.count("underscore");
            putInFolder = vm.count("folder");
            verbose = vm.count("verbose");
            
            if (vm.count("index"))
                indices = vm["index"].as<std::vector<int>>();
            
            const auto path = boost::filesystem::absolute(vm["file"].as<std::string>());
            
			if (!boost::filesystem::exists(path))
			{
				std::cerr << "File '" << path.string() << "' does not exist" << std::endl;
				return 1;
			}

            if (vm.count("print"))
                return print(path);
            else
                return exportSongs(path, vm["output"].as<std::string>());
        } else {
            std::cout << desc << std::endl;
            return 0;
        }
    } catch (const boost::program_options::error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
    }

	return 0;
}
