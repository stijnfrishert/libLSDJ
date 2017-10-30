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
    unsigned char decompressed[SONG_DECOMPRESSED_SIZE];
    FILE* file = fopen("/Users/stijnfrishert/Desktop/uncompressed.hex", "rb");
    fread(decompressed, sizeof(decompressed), 1, file);
    fclose(file);
    
    lsdj_compress_to_file(decompressed, 512, 0, 191, "/Users/stijnfrishert/Desktop/compressedc.hex", NULL);
    
    return 0;
}
