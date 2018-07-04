//
//  cpp
//  liblsdj
//
//  Created by Stijn on 04/07/2018.
//

#include <iomanip>
#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "exporter.hpp"

namespace lsdj
{
    int handle_error(lsdj_error_t* error)
    {
        std::cerr << "ERROR: " << lsdj_error_get_c_str(error) << std::endl;
        lsdj_error_free(error);
        return 1;
    }
    
    bool compareCaseInsensitive(std::string str1, std::string str2)
    {
        std::transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
        std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
        return str1 == str2;
    }
    
    std::string Exporter::constructName(const lsdj_project_t* project)
    {
        char name[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        lsdj_project_get_name(project, name, sizeof(name));
        
        if (underscore)
            std::replace(name, name + 9, 'x', '_');
        
        return name;
    }
    
    void Exporter::exportProject(const lsdj_project_t* project, boost::filesystem::path folder, bool workingMemory, lsdj_error_t** error)
    {
        auto name = constructName(project);
        if (name.empty())
            name = "(EMPTY)";
        
        if (putInFolder)
            folder /= name;
        boost::filesystem::create_directories(folder);
        
        std::stringstream stream;
        stream << name << convertVersionToString(lsdj_project_get_version(project), true);
        
        if (workingMemory)
            stream << ".WM";
        
        stream << ".lsdsng";
        folder /= stream.str();
        
        lsdj_project_write_lsdsng_to_file(project, folder.string().c_str(), error);
    }
    
    // Export all songs of a file
    int Exporter::exportSongs(const boost::filesystem::path& path, const std::string& output)
    {
        // Load in the save file
        lsdj_error_t* error = nullptr;
        lsdj_sav_t* sav = lsdj_sav_read_from_file(path.string().c_str(), &error);
        if (sav == nullptr)
            return handle_error(error);
        
        if (verbose)
            std::cout << "Read '" << path.string() << "'" << std::endl;
        
        const auto outputFolder = boost::filesystem::absolute(output);
        
        // If no specific indices were given, or -w was flagged (index == -1),
        // display the working memory song as well
        if ((indices.empty() && names.empty()) || std::find(std::begin(indices), std::end(indices), -1) != std::end(indices))
        {
            lsdj_project_t* project = lsdj_project_new_from_working_memory_song(sav, &error);
            if (error)
            {
                lsdj_sav_free(sav);
                return handle_error(error);
            }
            
            exportProject(project, outputFolder, true, &error);
            if (error)
            {
                lsdj_sav_free(sav);
                return handle_error(error);
            }
        }
        
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
                const auto namestr = std::string(name);
                if (std::find_if(std::begin(names), std::end(names), [&](const auto& x){ return compareCaseInsensitive(x, namestr); }) == std::end(names))
                    continue;
            }
            
            // Does this project contain a song? If not, it's empty
            if (lsdj_project_get_song(project) == NULL)
                continue;
            
            // Export the project
            exportProject(project, outputFolder, false, &error);
            if (error)
            {
                lsdj_sav_free(sav);
                return handle_error(error);
            }
            
            // Let the user know if verbose output has been toggled on
            if (verbose)
            {
                std::cout << "Exported " << constructName(project);
                
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
        
        lsdj_sav_free(sav);
        
        return 0;
    }
    
    int Exporter::print(const boost::filesystem::path& path)
    {
        // Try and read the sav
        lsdj_error_t* error = nullptr;
        lsdj_sav_t* sav = lsdj_sav_read_from_file(path.string().c_str(), &error);
        if (sav == nullptr)
            return lsdj::handle_error(error);
        
        // Header
        std::cout << "#   Name     ";
        if (versionStyle != VersionStyle::NONE)
            std::cout << "Ver    ";
        std::cout << "Fmt" << std::endl;
        
        // If no specific indices were given, or -w was flagged (index == -1),
        // display the working memory song as well
        if ((indices.empty() && names.empty()) || std::find(std::begin(indices), std::end(indices), -1) != std::end(indices))
        {
            printWorkingMemorySong(sav);
        }
        
        // Go through all compressed projects
        const auto count = lsdj_sav_get_project_count(sav);
        for (int i = 0; i < count; ++i)
        {
            // If indices were specified and this project wasn't one of them, move on to the next
            if (!indices.empty() && std::find(std::begin(indices), std::end(indices), i) == std::end(indices))
                continue;
            
            printProject(sav, i);
        }
        
        return 0;
    }
    
    std::string Exporter::convertVersionToString(unsigned char version, bool prefixDot) const
    {
        std::ostringstream stream;
        
        switch (versionStyle)
        {
            case VersionStyle::NONE:
                break;
            case VersionStyle::HEX:
                if (prefixDot)
                    stream << '.';
                stream << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned int>(version);
                break;
            case VersionStyle::DECIMAL:
                if (prefixDot)
                    stream << '.';
                stream << std::setfill('0') << std::setw(3) << static_cast<unsigned int>(version);
                break;
        }
        
        return stream.str();
    }
    
    void Exporter::printWorkingMemorySong(const lsdj_sav_t* sav)
    {
        std::cout << "WM  ";
        
        // If the working memory song represent one of the projects, display that name
        const auto active = lsdj_sav_get_active_project(sav);
        if (active != LSDJ_NO_ACTIVE_PROJECT)
        {
            lsdj_project_t* project = lsdj_sav_get_project(sav, active);
            
            const auto name = constructName(project);
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
        const lsdj_song_t* song = lsdj_sav_get_working_memory_song(sav);
        if (lsdj_song_get_file_changed_flag(song))
        {
            switch (versionStyle)
            {
                case VersionStyle::NONE: break;
                case VersionStyle::HEX:
                    std::cout << (lsdj_song_get_file_changed_flag(song) ? "*" : " ") << "  \t";
                    break;
                case VersionStyle::DECIMAL:
                    std::cout << (lsdj_song_get_file_changed_flag(song) ? "*" : " ") << "  \t";
                    break;
            }
        }
        
        // Display the format version of the song
        std::cout << std::to_string(lsdj_song_get_format_version(song)) << std::endl;
    }

    void Exporter::printProject(const lsdj_sav_t* sav, std::size_t index)
    {
        // Retrieve the project
        const lsdj_project_t* project = lsdj_sav_get_project(sav, index);
        
        // See if we're using name-based specification and whether this project has been singled out
        // If not, skip it and move on to the next one
        if (!names.empty())
        {
            char name[9];
            std::fill_n(name, 9, '\0');
            lsdj_project_get_name(project, name, sizeof(name));
            const auto namestr = std::string(name);
            if (std::find_if(std::begin(names), std::end(names), [&](const auto& x){ return lsdj::compareCaseInsensitive(x, namestr); }) == std::end(names))
                return;
        }
        
        // Retrieve the song belonging to this project, make sure it's there
        const lsdj_song_t* song = lsdj_project_get_song(project);
        if (!song)
            return;
        
        // Print out the index
        std::cout << std::to_string(index) << "  ";
        if (index < 10)
            std::cout << ' ';
        
        // Display the name of the project
        const auto name = constructName(project);
        std::cout << name;
        
        for (auto i = 0; i < (9 - name.length()); ++i)
            std::cout << ' ';
        
        // Dipslay the version number of the project
        std::cout << convertVersionToString(lsdj_project_get_version(project), false);
        switch (versionStyle)
        {
            case VersionStyle::NONE: break;
            case VersionStyle::HEX: std::cout << " \t"; break;
            case VersionStyle::DECIMAL: std::cout << "\t"; break;
        }
        
        // Retrieve the sav format version of the song and display it as well
        std::cout << std::to_string(lsdj_song_get_format_version(song)) << std::endl;
    }
}
