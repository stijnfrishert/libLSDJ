//
//  exporter.hpp
//  liblsdj
//
//  Created by Stijn on 04/07/2018.
//

#ifndef LSDJ_EXPORTER_HPP
#define LSDJ_EXPORTER_HPP

#include <boost/filesystem/path.hpp>

#include "../liblsdj/error.h"
#include "../liblsdj/project.h"
#include "../liblsdj/sav.h"

namespace lsdj
{
    int handle_error(lsdj_error_t* error);
    bool compareCaseInsensitive(std::string str1, std::string str2);
    
    class Exporter
    {
    public:
        enum class VersionStyle
        {
            NONE,
            HEX,
            DECIMAL
        };
        
    public:
        std::string constructName(const lsdj_project_t* project);
        void exportProject(const lsdj_project_t* project, boost::filesystem::path folder, bool workingMemory, lsdj_error_t** error);
        int exportSongs(const boost::filesystem::path& path, const std::string& output);
        int print(const boost::filesystem::path& path);
        
    public:
        // The version exporting style
        VersionStyle versionStyle = VersionStyle::HEX;
        
        bool underscore = false;
        bool putInFolder = false;
        bool verbose = false;
        
        std::vector<int> indices;
        std::vector<std::string> names;
    };
}

#endif
