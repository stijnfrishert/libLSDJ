#include <stdio.h>
#include <string.h>

#include "sav.h"

int main(int argc, char* argv[])
{
//    lsdj_sav_t sav;
//    lsdj_error_t* error = NULL;
//    lsdj_read_sav_from_file("/Users/stijnfrishert/Desktop/LSDj/4ntler/Chipwrecked Set.sav", &sav, &error);
    
    FILE* file = fopen("/Users/stijnfrishert/Desktop/LSDj/LSDj/lsdj_5.6.4.sav", "r");
    fseek(file, 0, SEEK_END);
    const size_t size = (size_t)ftell(file);
    unsigned char inData[size];
    fseek(file, 0, SEEK_SET);
    fread(inData, size, 1, file);
    fclose(file);
    
    // Read
    lsdj_sav_t sav;
    memset(&sav, 0, sizeof(sav));
    lsdj_error_t* error = NULL;
    lsdj_read_sav_from_memory(inData, size, &sav, &error);
    
    if (error)
    {
        printf("%s\n", lsdj_get_error_c_str(error));
        lsdj_free_error(error);
        return 1;
    }
    
    // Write
    unsigned char outData[size];
    memset(outData, 0, size);
    lsdj_write_sav_to_memory(&sav, outData, size, &error);
    
    if (error)
    {
        printf("%s", lsdj_get_error_c_str(error));
        lsdj_free_error(error);
        return 1;
    }
    
    // Compare the original and write
//    for (int i = 0; i < SONG_DECOMPRESSED_SIZE; ++i)
//    {
//        if (outData[i] != inData[i])
//        {
//            printf("diff at %d\n", i);
//            return 1;
//        }
//    }
    
    lsdj_write_sav_to_file(&sav, "/Users/stijnfrishert/Desktop/LSDj/LSDj/out_lsdj_5.6.4.sav", &error);
    
    
    
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
