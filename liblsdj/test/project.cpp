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
    const auto lsdsng = readFileContents(RESOURCES_FOLDER "lsdsng/happy_birthday.lsdsng");
    
    const auto raw = readFileContents(RESOURCES_FOLDER "raw/happy_birthday.raw");
    assert(raw.size() == LSDJ_SONG_BUFFER_BYTE_COUNT);
    
	SECTION( "Reading an .lsdsng from memory" )
	{
        // Read the lsdsng from memory
		auto project = lsdj_project_read_lsdsng_from_memory(lsdsng.data(), lsdsng.size(), nullptr);
		REQUIRE( project != nullptr );

        // Name
		std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
		lsdj_project_get_name(project, name.data());
		REQUIRE( memcmp(name.data(), "HAPPY BD", LSDJ_PROJECT_NAME_LENGTH) == 0 );

        // Version
		REQUIRE( lsdj_project_get_version(project) == 4 );

        // Song data
		auto buffer = lsdj_project_get_song_buffer(project);
		std::ofstream stream("/Users/stijn/Desktop/hb.song");
		stream.write(reinterpret_cast<const char*>(buffer->bytes), LSDJ_SONG_BUFFER_BYTE_COUNT);

        // Read raw and compare
		REQUIRE( memcmp(raw.data(), lsdj_project_get_song_buffer(project)->bytes, LSDJ_SONG_BUFFER_BYTE_COUNT) == 0 );

        // Clean up
		lsdj_project_free(project);
	}

	SECTION( "Reading an .lsdsng from file" )
	{
        // Read the lsdsng from file
		auto project = lsdj_project_read_lsdsng_from_file(RESOURCES_FOLDER "lsdsng/happy_birthday.lsdsng", nullptr);
		REQUIRE( project != nullptr );

        // Name
		std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
		lsdj_project_get_name(project, name.data());
		REQUIRE( memcmp(name.data(), "HAPPY BD", LSDJ_PROJECT_NAME_LENGTH) == 0 );

        // Version
		REQUIRE( lsdj_project_get_version(project) == 4 );

        // Song data
		auto buffer = lsdj_project_get_song_buffer(project);
		std::ofstream stream("/Users/stijn/Desktop/hb.song");
		stream.write(reinterpret_cast<const char*>(buffer->bytes), LSDJ_SONG_BUFFER_BYTE_COUNT);

        // Read raw and compare
		REQUIRE( memcmp(raw.data(), lsdj_project_get_song_buffer(project)->bytes, LSDJ_SONG_BUFFER_BYTE_COUNT) == 0 );

        // Clean up
		lsdj_project_free(project);
	}

//	SECTION( "Writing an .lsdsng to memory")
//	{
//        // Create the project
//		auto project = lsdj_project_new(nullptr);
//		REQUIRE( project != nullptr );
//
//        lsdj_project_set_name(project, "HAPPY BD", 8);
//        lsdj_project_set_version(project, 4);
//        
//        lsdj_song_buffer_t songBuffer;
//        std::copy_n(raw.data(), raw.size(), songBuffer.bytes);
//        lsdj_project_set_song_buffer(project, &songBuffer);
//        
//        // Write it to memory
//        std::array<unsigned char, LSDSNG_MAX_SIZE> data;
//        data.fill(0);
////        auto writeCount = lsdj_project_write_lsdsng_to_memory(project, data.data(), nullptr);
//        auto writeCount = lsdj_project_write_lsdsng_to_file(project, "/Users/stijn/Desktop/hbd.lsdsng", nullptr);
//        
//        REQUIRE( memcmp(lsdsng.data(), data.data(), writeCount) == 0 );
//
//		lsdj_project_free(project);
//	}
}
