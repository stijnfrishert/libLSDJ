#include <song.h>

#include <catch2/catch.hpp>

using namespace Catch;

SCENARIO( "Song", "[song]" )
{
	REQUIRE(LSDJ_SONG_BYTE_COUNT == 0x8000);

	GIVEN( "A song is created" )
	{
		lsdj_song_t song;

		REQUIRE(sizeof(song.bytes) == 0x8000);
	}
}