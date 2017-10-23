#include <boost/filesystem.hpp>
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

    lsdj_error_t* error = nullptr;
    lsdj_sav_t* sav = lsdj_read_sav_from_file(boost::filesystem::canonical(argv[1]).c_str(), &error);
    if (sav == nullptr)
        return 1;
    
    if (error)
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
        
        char name[9];
        lsdj_project_get_name(project, name, sizeof(name));
        
        std::stringstream stream;
        stream << name;
        stream << ".lsdsng";
        
        lsdj_write_lsdsng_to_file(project, (cwd / stream.str()).c_str(), &error);
        if (error)
            return handle_error(error);
    }
    
    lsdj_free_sav(sav);

	return 0;
}
