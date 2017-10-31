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
    lsdj_sav_t* sav = lsdj_read_sav_from_file("/Users/stijn/Desktop/lsdj/lsdj/in.sav", &error);
    lsdj_write_sav_to_file(sav, "/Users/stijn/Desktop/lsdj/lsdj/out.sav", &error);
    
    return 0;
}
