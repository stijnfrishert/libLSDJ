#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>

#include "../liblsdj/compression.h"
#include "../liblsdj/sav.h"
#include "../liblsdj/vio.h"

int main(int argc, char* argv[])
{
    lsdj_error_t* error = nullptr;
    const auto project = lsdj_read_lsdsng_from_file("/Users/stijn/Downloads/crazy1.lsdsng", &error);
    if (error)
    {
        std::cerr << lsdj_get_error_c_str(error) << std::endl;
        lsdj_free_error(error);
        return 1;
    }
    
    lsdj_write_lsdsng_to_file(project, "/Users/stijn/Desktop/out.lsdsng", &error);
    if (error)
    {
        std::cerr << lsdj_get_error_c_str(error) << std::endl;
        lsdj_free_error(error);
        return 1;
    }
    
    const auto sav = lsdj_read_sav_from_file("/Users/stijn/Desktop/lsdj/4ntler/Those Eyes.sav", &error);
    if (error)
    {
        std::cerr << lsdj_get_error_c_str(error) << std::endl;
        lsdj_free_error(error);
        return 1;
    }
    
    lsdj_write_sav_to_file(sav, "/Users/stijn/Desktop/out.sav", &error);
    if (error)
    {
        std::cerr << lsdj_get_error_c_str(error) << std::endl;
        lsdj_free_error(error);
        return 1;
    }
    
    return 0;
}
