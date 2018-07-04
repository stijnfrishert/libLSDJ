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
        int exportProjects(const boost::filesystem::path& path, const std::string& output);
        void exportProject(const lsdj_project_t* project, boost::filesystem::path folder, bool workingMemory, lsdj_error_t** error);
        int print(const boost::filesystem::path& path);
        
    public:
        // The version exporting style
        VersionStyle versionStyle = VersionStyle::HEX;
        
        bool underscore = false;
        bool putInFolder = false;
        bool verbose = false;
        
        std::vector<int> indices;
        std::vector<std::string> names;
        
    private:
        // Converts a project version to a string representation using the current VersionStyle
        std::string convertVersionToString(unsigned char version, bool prefixDot) const;
        
        // Print the working memory song line
        void printWorkingMemorySong(const lsdj_sav_t* sav);
        
        // Print a sav project line
        void printProject(const lsdj_sav_t* sav, std::size_t index);
        
        std::string constructName(const lsdj_project_t* project);

    };
}

#endif
