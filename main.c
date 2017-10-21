#include <stdio.h>
#include <string.h>

#include "sav.h"

int main(int argc, char* argv[])
{
//    lsdj_sav_t sav;
//    lsdj_error_t* error = NULL;
//    lsdj_read_sav_from_file("/Users/stijnfrishert/Desktop/LSDj/4ntler/Chipwrecked Set.sav", &sav, &error);
    
    FILE* file = fopen("/Users/stijnfrishert/Desktop/LSDj/4ntler/Chipwrecked Set.sav", "r");
    fseek(file, 0, SEEK_END);
    const size_t size = (size_t)ftell(file);
    unsigned char inData[size];
    fseek(file, 0, SEEK_SET);
    fread(inData, size, 1, file);
    fclose(file);
    
    lsdj_sav_t sav;
    lsdj_error_t* error = NULL;
    lsdj_read_sav_from_memory(inData, size, &sav, &error);
    
    if (error)
    {
        printf("%s", lsdj_get_error_c_str(error));
        lsdj_free_error(error);
        return 1;
    }
    
    unsigned char outData[size];
    memset(outData, 0, size);
    lsdj_write_sav_to_memory(&sav, outData, size, &error);
    
    // Compare the original and write
    for (int i = 0; i < size; ++i)
    {
        if (outData[i] != inData[i])
            return 1;
    }
    
    
    
//    unsigned char write[0xF4240];
//    memset(write, 0, sizeof(write));
//    lsdj_write_sav_to_memory(&sav, write, NULL);
    
//    lsdj_write_sav(sav, "lsdj.sav", NULL);
    
//	for (int i = 1; i < argc; ++i)
//	{
//		lsdj_sav_t* sav = lsdj_open_sav(argv[i], NULL);
    
//        for (int i = 0; i < 32; ++i)
//        {
//            if (sav->projects[i].song == NULL)
//                continue;
//            
//            char v[3];
//            sprintf(v, "%02X", sav->projects[i].version);
//            v[2] = 0;
//            
//            char path[32];
//            memset(path, 0, 32);
//            strncat(path, sav->projects[i].name, 8);
//
//            strcat(path, ".");
//            strcat(path, v);
//            strcat(path, ".lsdsng");
//            
//            lsdj_write_lsdsng(&sav->projects[i], p.ath, NULL);
//        }
//
//        if (sav)
//            lsdj_free_sav(sav);
//	}

	return 0;
}
