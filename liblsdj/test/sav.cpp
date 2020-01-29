#include <sav.h>

#include <algorithm>
#include <array>
#include <catch2/catch.hpp>

using namespace Catch;

SCENARIO( "Saves", "[sav]" )
{
	std::array<unsigned char, LSDJ_SONG_BUFFER_BYTE_COUNT> zeroBuffer;
	zeroBuffer.fill(0);

	REQUIRE(LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX == 0xFF);

	GIVEN( "A new sav is created" )
	{
		auto sav = lsdj_sav_new(nullptr);
		REQUIRE(sav != nullptr);

		WHEN( "The working memory song buffer is requested" )
		{
			auto songBuffer = lsdj_sav_get_working_memory_song_buffer(sav);
			REQUIRE(songBuffer != nullptr);

			THEN( "It should be a zeroed out array of bytes" )
			{
				REQUIRE(memcmp(songBuffer->bytes, zeroBuffer.data(), LSDJ_SONG_BUFFER_BYTE_COUNT) == 0);
			}
		}

		WHEN( "Changing the working memory song buffer" )
		{
			lsdj_song_buffer_t newBuffer;
			memset(newBuffer.bytes, 1, sizeof(newBuffer.bytes));

			lsdj_sav_set_working_memory_song_buffer(sav, &newBuffer);

			THEN( "It should update accordingly" )
			{
				auto result = lsdj_sav_get_working_memory_song_buffer(sav);
				REQUIRE(memcpy(&newBuffer.bytes, result->bytes, LSDJ_SONG_BUFFER_BYTE_COUNT));
			}
		}

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