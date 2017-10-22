#include <boost/filesystem.hpp>

#include <iostream>
#include <sstream>

#include "../liblsdj/sav.h"

int handle_error(lsdj_error_t* error)
{
    std::cerr << "ERROR: " << lsdj_get_error_c_str(error) << std::endl;
    lsdj_free_error(error);
    return 1;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
        return 1;
    
    lsdj_sav_t* sav = lsdj_create_sav();
    lsdj_error_t* error = NULL;
    lsdj_read_sav_from_file(argv[1], sav, &error);
    
    if (error)
        return handle_error(error);
    
    const auto& cwd = boost::filesystem::current_path();
    
    for (int i = 0; i < PROJECT_COUNT; ++i)
    {
        lsdj_project_t& project = sav->projects[i];
        if (!project.song)
            continue;
        
        std::stringstream stream;
        stream << project.name;
        stream << ".lsdsng";
        
        lsdj_write_lsdsng_to_file(&project, (cwd / stream.str()).c_str(), &error);
        if (error)
            return handle_error(error);
    }

	return 0;
}
