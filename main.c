#include <stdio.h>

 #include "sav.h"

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		lsdj_sav_t* sav = lsdj_open_sav(argv[i], NULL);

		if (sav)
			lsdj_close_sav(sav);
	}

	return 0;
}
