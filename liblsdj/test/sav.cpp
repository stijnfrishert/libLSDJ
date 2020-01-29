#include <sav.h>

#include <catch2/catch.hpp>

using namespace Catch;

SCENARIO( "Saves", "[sav]" )
{
	GIVEN( "A new sav is created" )
	{
		auto sav = lsdj_sav_new(nullptr);
		REQUIRE(sav != nullptr);

		WHEN( "The active project slot index is requested" )
		{
			auto index = lsdj_sav_get_active_project_index(sav);

			THEN( "It should not refer to any project slot" )
			{
				REQUIRE(index == LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX);
			}
		}

		WHEN( "Changing the active project slot index" )
		{
			REQUIRE(lsdj_sav_get_active_project_index(sav) != 11);
			lsdj_sav_set_active_project_index(sav, 11);

			THEN( "It should update accordingly" )
			{
				REQUIRE(lsdj_sav_get_active_project_index(sav) == 11);
			}
		}
	}
}