/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 Stijn Frishert
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 */

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
std::vector<std::string> names;

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

// Export all songs of a file
int exportSongs(const boost::filesystem::path& path, const std::string& output)
{
    // Load in the save file
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
    
    // Go through every project
    const auto count = lsdj_sav_get_project_count(sav);
    for (int i = 0; i < count; ++i)
    {
        // See if we're using indices and this project hasn't been specified
        // If so, skip it and move on to the next one
        if (!indices.empty() && std::find(std::begin(indices), std::end(indices), i) == std::end(indices))
            continue;
        
        // Retrieve the project
        lsdj_project_t* project = lsdj_sav_get_project(sav, i);
        
        // See if we're using name-based specification and whether this project has been singled out
        // If not, skip it and move on to the next one
        if (!names.empty())
        {
            char name[9];
            std::fill_n(name, 9, '\0');
            lsdj_project_get_name(project, name, sizeof(name));
            const auto c = std::string(name);
            if (std::find(std::begin(names), std::end(names), std::string(name)) == std::end(names))
                continue;
        }

        // Export the project
        exportProject(project, outputFolder, versionStyle, underscore, putInFolder, &error);
        if (error)
            return handle_error(error);
        
        // Let the user know if verbose output has been toggled on
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

// Print the contents of a file
int print(const boost::filesystem::path& path)
{
    // Try and read the sav
    lsdj_error_t* error = nullptr;
    lsdj_sav_t* sav = lsdj_read_sav_from_file(path.string().c_str(), &error);
    if (sav == nullptr)
    {
        lsdj_free_sav(sav);
        return handle_error(error);
    }
    
    // Header
    std::cout << "#   Name     ";
    if (versionStyle != VersionStyle::NONE)
        std::cout << "Ver  ";
    std::cout << "Fmt" << std::endl;

    // If no specific indices were given, or -w was flagged (index == -1),
    // display the working memory song as well
    if ((indices.empty() && names.empty()) || std::find(std::begin(indices), std::end(indices), -1) != std::end(indices))
    {
        std::cout << "WM. ";
        
        // If the working memory song represent one of the projects, display that name
        const auto active = lsdj_sav_get_active_project(sav);
        if (active != 0xFF)
        {
            lsdj_project_t* project = lsdj_sav_get_project(sav, active);
            
            const auto name = constructName(project, underscore);
            std::cout << name;
            for (auto i = 0; i < (9 - name.length()); ++i)
                std::cout << ' ';
        } else {
            // The working memory doesn't represent one of the projects, so it
            // doesn't really have a name
            std::cout << "         ";
        }
        
        // Display whether the working memory song is "dirty"/edited, and display that
        // as version number (it doesn't really have a version number otherwise)
        const lsdj_song_t* song = lsdj_sav_get_song(sav);
        if (versionStyle != VersionStyle::NONE)
        {
            if (lsdj_song_get_file_changed_flag(song))
                std::cout << "*    ";
            else
                std::cout << "      ";
        }
        
        // Display the format version of the song
        std::cout << std::to_string(lsdj_song_get_format_version(song));
    
        std::cout << std::endl;
    }
    
    // Go through all compressed projects
    const auto count = lsdj_sav_get_project_count(sav);
    for (int i = 0; i < count; ++i)
    {
        // If indices were specified and this project wasn't one of them, move on to the next
        if (!indices.empty() && std::find(std::begin(indices), std::end(indices), i) == std::end(indices))
            continue;
        
        // Retrieve the project
        const lsdj_project_t* project = lsdj_sav_get_project(sav, i);
        
        // See if we're using name-based specification and whether this project has been singled out
        // If not, skip it and move on to the next one
        if (!names.empty())
        {
            char name[9];
            std::fill_n(name, 9, '\0');
            lsdj_project_get_name(project, name, sizeof(name));
            const auto c = std::string(name);
            if (std::find(std::begin(names), std::end(names), std::string(name)) == std::end(names))
                continue;
        }
        
        // Retrieve the song belonging to this project, make sure it's there
        const lsdj_song_t* song = lsdj_project_get_song(project);
        if (!song)
            continue;
        
        // Print out the index
        std::cout << std::to_string(i) << ". ";
        if (i < 10)
            std::cout << ' ';
        
        // Display the name of the project
        const auto name = constructName(project, underscore);
        std::cout << name;
        
        for (auto i = 0; i < (9 - name.length()); ++i)
            std::cout << ' ';
        
        // Dipslay the version number of the project
        switch (versionStyle)
        {
            case VersionStyle::NONE: break;
            case VersionStyle::HEX:
                std::cout << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)lsdj_project_get_version(project) << "   ";
                break;
            case VersionStyle::DECIMAL:
                std::cout << std::setfill('0') << std::setw(3) << (unsigned int)lsdj_project_get_version(project) << "  ";
                break;
        }
        
        // Retrieve the sav format version of the song and display it as well
        std::cout << std::to_string(lsdj_song_get_format_version(song));
        
        std::cout << std::endl;
    }
    
    return 0;
}

int main(int argc, char* argv[])
{
    // Setup the command-line options
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help screen")
        ("file", boost::program_options::value<std::string>(), "Input save file, can be a nameless option")
        ("noversion", "Don't add version numbers to the filename")
        ("folder,f", "Put every lsdsng in its own folder")
        ("print,p", "Print a list of all songs in the sav")
        ("decimal,d", "Use decimal notation for the version number, instead of hex")
        ("underscore,u", "Use an underscore for the special lightning bolt character, instead of x")
        ("output,o", boost::program_options::value<std::string>()->default_value(""), "Output folder for the lsdsng's")
        ("verbose,v", "Verbose output during export")
        ("index,i", boost::program_options::value<std::vector<int>>(), "Single out a given project index to export, 0 or more")
        ("name,n", boost::program_options::value<std::vector<std::string>>(), "Single out a given project by name to export")
        ("working-memory,w", "Single out the working-memory song to export");
    
    // Set up the input file command-line argument
    boost::program_options::positional_options_description positionalOptions;
    positionalOptions.add("file", 1);
    
    try
    {
        // Parse the command-line options
        boost::program_options::variables_map vm;
        boost::program_options::command_line_parser parser(argc, argv);
        parser = parser.options(desc);
        parser = parser.positional(positionalOptions);
        boost::program_options::store(parser.run(), vm);
        boost::program_options::notify(vm);
        
        // Show help if requested
        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        // Do we have an input file?
        } else if (vm.count("file")) {
            // What is the path of the input file, and does it exist on disk?
            const auto path = boost::filesystem::absolute(vm["file"].as<std::string>());
            if (!boost::filesystem::exists(path))
            {
                std::cerr << "File '" << path.string() << "' does not exist" << std::endl;
                return 1;
            }
            
            // Parse some of the flags manipulating output "style"
            versionStyle = vm.count("noversion") ? VersionStyle::NONE : vm.count("decimal") ? VersionStyle::DECIMAL : VersionStyle::HEX;
            underscore = vm.count("underscore");
            putInFolder = vm.count("folder");
            verbose = vm.count("verbose");
            
            // Has the user specified one or more specific indices to export?
            if (vm.count("index"))
                indices = vm["index"].as<std::vector<int>>();
            if (vm.count("working-memory"))
                indices.emplace_back(-1); // -1 represents working memory, kind-of a hack, but meh :/
            if (vm.count("name"))
                names = vm["name"].as<std::vector<std::string>>();

            // Has the user requested a print, or an actual export?
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
