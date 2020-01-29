#include <song_buffer.h>

#include <catch2/catch.hpp>

using namespace Catch;

SCENARIO( "Song Buffer", "[song_buffer]" )
{
	REQUIRE(LSDJ_SONG_BUFFER_BYTES_COUNT == 0x8000);

	GIVEN( "A song buffer is created" )
	{
		lsdj_song_buffer_t buffer;

		REQUIRE(sizeof(buffer.bytes) == 0x8000);
	}
}