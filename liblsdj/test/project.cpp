#include <project.h>

#include <algorithm>
#include <array>
#include <catch2/catch.hpp>

using namespace Catch;

SCENARIO( "Project", "[project]" )
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

		lsdj_project_free(project);
	}
}