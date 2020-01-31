#include <project.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <catch2/catch.hpp>
#include <fstream>

#include "file.hpp"

using namespace Catch;

SCENARIO( "Project creation and querying", "[project]" )
{
	REQUIRE(LSDJ_SONG_BUFFER_BYTE_COUNT == 0x8000);

	GIVEN( "A new project is created" )
	{
		auto project = lsdj_project_new(nullptr);
		REQUIRE(project != nullptr);

		WHEN( "Requesting the name")
		{
			std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
			lsdj_project_get_name(project, name.data());

			THEN( "The name should be empty")
			{
				REQUIRE_THAT(name.data(), Equals(""));
				REQUIRE(lsdj_project_get_name_length(project) == 0);
			}
		}

		WHEN( "Setting the name" )
		{
			lsdj_project_set_name(project, "NAME", 4);

			THEN( "The name should have changed accordingly" )
			{
				std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
				lsdj_project_get_name(project, name.data());

				REQUIRE_THAT(name.data(), Equals("NAME"));
				REQUIRE(lsdj_project_get_name_length(project) == 4);
			}
		}

		WHEN( "Requesting the version number")
		{
			auto version = lsdj_project_get_version(project);

			THEN( "The version should be 0" )
			{
				REQUIRE(version == 0);
			}
		}

		WHEN( "Changing the version number" )
		{
			REQUIRE(lsdj_project_get_version(project) != 34);
			
			lsdj_project_set_version(project, 34);

			THEN( "The version should update ")
			{
				REQUIRE(lsdj_project_get_version(project) == 34);
			}
		}

		WHEN( "Requesting the song buffer" )
		{
			auto buffer = lsdj_project_get_song_buffer(project);

			THEN( "It should be an zeroed out song buffer" )
			{
				std::array<unsigned char, LSDJ_SONG_BUFFER_BYTE_COUNT> zeroes;
				zeroes.fill(0);

				REQUIRE(memcmp(buffer->bytes, zeroes.data(), LSDJ_SONG_BUFFER_BYTE_COUNT) == 0);
			}
		}

		WHEN( "Changing a song buffer" )
		{
			lsdj_song_buffer_t buffer;
			std::fill_n(buffer.bytes, LSDJ_SONG_BUFFER_BYTE_COUNT, 1);
			lsdj_project_set_song_buffer(project, &buffer);

			THEN( "The song buffer should have been updated accordingly" )
			{
				auto project_buffer = lsdj_project_get_song_buffer(project);
				REQUIRE(memcmp(buffer.bytes, project_buffer->bytes, sizeof(buffer.bytes)) == 0);
			}
		}

		WHEN( "Copying a project" )
		{
			lsdj_project_set_name(project, "MYSONG", 6);
			lsdj_project_set_version(project, 16);

			lsdj_song_buffer_t buffer;
			std::fill_n(buffer.bytes, LSDJ_SONG_BUFFER_BYTE_COUNT, 40);
			lsdj_project_set_song_buffer(project, &buffer);

			auto copy = lsdj_project_copy(project, nullptr);
			REQUIRE(copy != nullptr);
				
			THEN( "The data should remain intact" )
			{
				std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
				lsdj_project_get_name(copy, name.data());

				REQUIRE_THAT(name.data(), Equals("MYSONG"));
				REQUIRE(lsdj_project_get_version(copy) == 16);

				auto bufferCopy = lsdj_project_get_song_buffer(project);
				REQUIRE(memcmp(buffer.bytes, bufferCopy->bytes, LSDJ_SONG_BUFFER_BYTE_COUNT) == 0);
			}
		}

		lsdj_project_free(project);
	}
}

TEST_CASE( ".lsdsng save/load", "[project]" )
{
	SECTION( "Reading reading an .lsdsng from file" )
	{
		auto project = lsdj_project_read_lsdsng_from_file(RESOURCES_FOLDER "lsdsng/happy_birthday.lsdsng", nullptr);
		REQUIRE( project != nullptr );

		std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
		lsdj_project_get_name(project, name.data());
		REQUIRE( memcmp(name.data(), "HAPPY BD", LSDJ_PROJECT_NAME_LENGTH) == 0 );

		REQUIRE( lsdj_project_get_version(project) == 2 );

		// auto buffer = lsdj_project_get_song_buffer(project);
		// std::ofstream stream("/Users/stijn/Desktop/hb.song");
		// stream.write(reinterpret_cast<const char*>(buffer->bytes), LSDJ_SONG_BUFFER_BYTE_COUNT);

		auto raw = readFileContents(RESOURCES_FOLDER "raw/happy_birthday.raw");
		assert(raw.size() == LSDJ_SONG_BUFFER_BYTE_COUNT);

		REQUIRE( memcmp(raw.data(), lsdj_project_get_song_buffer(project)->bytes, LSDJ_SONG_BUFFER_BYTE_COUNT) == 0 );

		lsdj_project_free(project);
	}
}