#include <stdio.h>
#include <string.h>

 #include "sav.h"

int main(int argc, char* argv[])
{
    lsdj_sav_t* sav = lsdj_open_sav("/Users/stijn/Desktop/lsdj/lsdj_old.sav", NULL);
    
    lsdj_write_sav(sav, "lsdj.sav", NULL);
    
	for (int i = 1; i < argc; ++i)
	{
		lsdj_sav_t* sav = lsdj_open_sav(argv[i], NULL);
        
        for (int i = 0; i < 32; ++i)
        {
            if (sav->projects[i].song == NULL)
                continue;
            
            char v[3];
            sprintf(v, "%02X", sav->projects[i].version);
            v[2] = 0;
            
            char path[32];
            memset(path, 0, 32);
            strncat(path, sav->projects[i].name, 8);

            strcat(path, ".");
            strcat(path, v);
            strcat(path, ".lsdsng");
            
            lsdj_write_lsdsng(&sav->projects[i], path, NULL);
        }

		if (sav)
			lsdj_free_sav(sav);
	}

	return 0;
}
