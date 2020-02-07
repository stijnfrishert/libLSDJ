#include <sav.h>

#include <algorithm>
#include <array>
#include <catch2/catch.hpp>

#include "file.hpp"

using namespace Catch;

SCENARIO( "Saves", "[sav]" )
{
	std::array<unsigned char, LSDJ_SONG_BUFFER_BYTE_COUNT> zeroBuffer;
	zeroBuffer.fill(0);

	REQUIRE(LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX == 0xFF);

	// Sample project used in some tests
	auto project = lsdj_project_new(nullptr);
	lsdj_project_set_name(project, "MYSONG", 6);
	lsdj_project_set_version(project, 16);

	lsdj_song_buffer_t songBuffer;
	std::fill_n(songBuffer.bytes, LSDJ_SONG_BUFFER_BYTE_COUNT, 40);
	lsdj_project_set_song_buffer(project, &songBuffer);

	GIVEN( "A new sav is created" )
	{
		auto sav = lsdj_sav_new(nullptr);
		REQUIRE( sav != nullptr );

		WHEN( "The working memory song buffer is requested" )
		{
			auto songBuffer = lsdj_sav_get_working_memory_song(sav);
			REQUIRE( songBuffer != nullptr );

			THEN( "It should be a zeroed out array of bytes" )
			{
				REQUIRE( memcmp(songBuffer->bytes, zeroBuffer.data(), LSDJ_SONG_BUFFER_BYTE_COUNT) == 0 );
			}
		}

		WHEN( "Changing the working memory song buffer" )
		{
			lsdj_sav_set_working_memory_song(sav, &songBuffer);

			THEN( "Retrieving the song buffer should return the same data" )
			{
				auto result = lsdj_sav_get_working_memory_song(sav);
				REQUIRE( memcpy(&songBuffer.bytes, result->bytes, LSDJ_SONG_BUFFER_BYTE_COUNT) );
			}
		}

		WHEN( "Copying the working memory song from a project")
		{
			REQUIRE_FALSE( lsdj_sav_set_working_memory_song_from_project(sav, 2, nullptr) );

			lsdj_sav_set_project_move(sav, 2, project);
			lsdj_sav_set_working_memory_song_from_project(sav, 2, nullptr);		

			THEN( "The working memory and index should change" )
			{
				auto result = lsdj_sav_get_working_memory_song(sav);
				REQUIRE( memcpy(&songBuffer.bytes, result->bytes, LSDJ_SONG_BUFFER_BYTE_COUNT) );
			}
		}

		WHEN( "The active project slot index is requested" )
		{
			auto index = lsdj_sav_get_active_project_index(sav);

			THEN( "It should not refer to any project slot" )
			{
				REQUIRE( index == LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX );
			}
		}

		WHEN( "Changing the active project slot index" )
		{
			REQUIRE( lsdj_sav_get_active_project_index(sav) != 11 );
			lsdj_sav_set_active_project_index(sav, 11);

			THEN( "It should update accordingly" )
			{
				REQUIRE( lsdj_sav_get_active_project_index(sav) == 11 );
			}
		}

		WHEN( "Copying a project into a sav" )
		{
			lsdj_sav_set_project_copy(sav, 7, project, nullptr);

			THEN( "The data in the project should be identical " )
			{
				auto copy = lsdj_sav_get_project(sav, 7);
				REQUIRE( copy != nullptr );

				std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
				lsdj_project_get_name(copy, name.data());

				REQUIRE_THAT( name.data(), Equals("MYSONG") );
				REQUIRE( lsdj_project_get_version(copy) == 16 );

				auto bufferCopy = lsdj_project_get_song_buffer(project);
				REQUIRE( memcmp(songBuffer.bytes, bufferCopy->bytes, LSDJ_SONG_BUFFER_BYTE_COUNT) == 0 );
			}
		}

		WHEN( "Moving a project into a sav" )
		{
			lsdj_sav_set_project_move(sav, 3, project);

			THEN( "The project in the sav should point to the same memory " )
			{
				REQUIRE(lsdj_sav_get_project(sav, 3) == project);

				WHEN( "Erasing a project from a sav" )
				{
					lsdj_sav_erase_project(sav, 3);

					THEN( "The slot should be empty" )
					{
						REQUIRE( lsdj_sav_get_project(sav, 3) == nullptr );
					}
				}
			}
		}

		WHEN( "Requesting a project" )
		{
			auto project = lsdj_sav_get_project(sav, 0);

			THEN( "The project should be NULL" )
			{
				REQUIRE( project == NULL );
			}
		}

		WHEN( "Copying a sav" )
		{
			lsdj_sav_set_working_memory_song(sav, &songBuffer);
			lsdj_sav_set_active_project_index(sav, 15);
			lsdj_sav_set_project_move(sav, 9, project);

			THEN( "The data should remain intact" )
			{
				auto copy = lsdj_sav_copy(sav, nullptr);
				REQUIRE( copy != nullptr );

				auto copyBuffer = lsdj_sav_get_working_memory_song(sav);
				REQUIRE( memcpy(&songBuffer.bytes, copyBuffer->bytes, LSDJ_SONG_BUFFER_BYTE_COUNT) );
				REQUIRE( lsdj_sav_get_active_project_index(sav) == 15 );
				REQUIRE( lsdj_sav_get_project(sav, 9) == project );
			}
		}
	}
}

TEST_CASE( ".sav save/load", "[sav]" )
{
	const auto raw = readFileContents(RESOURCES_FOLDER "raw/happy_birthday.raw");
    assert(raw.size() == LSDJ_SONG_BUFFER_BYTE_COUNT);

    const auto save = readFileContents(RESOURCES_FOLDER "sav/happy_birthday.sav");
    assert(save.size() == 131072);

	SECTION( "Reading a .sav from file" )
	{
		auto sav = lsdj_sav_read_from_file(RESOURCES_FOLDER "sav/happy_birthday.sav", nullptr);
		REQUIRE( sav != nullptr );

		auto wm = lsdj_sav_get_working_memory_song(sav);
		REQUIRE( memcmp(wm->bytes, raw.data(), LSDJ_SONG_BUFFER_BYTE_COUNT) == 0 );

		REQUIRE( lsdj_sav_get_active_project_index(sav) == LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX );

		auto project = lsdj_sav_get_project(sav, 0);
		REQUIRE( project != nullptr );

		std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
		lsdj_project_get_name(project, name.data());
		REQUIRE( strncmp(name.data(), "HAPPY BD", LSDJ_PROJECT_NAME_LENGTH) == 0 );

		REQUIRE( lsdj_project_get_version(project) == 4 );

		auto song_buffer = lsdj_project_get_song_buffer(project);
		REQUIRE( memcmp(song_buffer->bytes, raw.data(), raw.size()) == 0 );

		lsdj_sav_free(sav);
	}

	SECTION( "Reading a .sav from memory" )
	{
		auto sav = lsdj_sav_read_from_memory(save.data(), save.size(), nullptr);
		REQUIRE( sav != nullptr );

		// Working memory
		auto wm = lsdj_sav_get_working_memory_song(sav);
		REQUIRE( memcmp(wm->bytes, raw.data(), LSDJ_SONG_BUFFER_BYTE_COUNT) == 0 );

		// Active project
		REQUIRE( lsdj_sav_get_active_project_index(sav) == LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX );

		auto project = lsdj_sav_get_project(sav, 0);
		REQUIRE( project != nullptr );

		std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
		lsdj_project_get_name(project, name.data());
		REQUIRE( strncmp(name.data(), "HAPPY BD", LSDJ_PROJECT_NAME_LENGTH) == 0 );

		REQUIRE( lsdj_project_get_version(project) == 4 );

		auto song_buffer = lsdj_project_get_song_buffer(project);
		REQUIRE( memcmp(song_buffer->bytes, raw.data(), raw.size()) == 0 );

		lsdj_sav_free(sav);
	}

	SECTION( "Writing a .sav to memory" )
	{
		auto sav = lsdj_sav_new(nullptr);
		REQUIRE( sav != nullptr );

		// Working memory
		lsdj_song_buffer_t wm;
		memcpy(wm.bytes, raw.data(), raw.size());
		lsdj_sav_set_working_memory_song(sav, &wm);

		// Active project
		lsdj_sav_set_active_project_index(sav, LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX);

		auto project = lsdj_project_read_lsdsng_from_file(RESOURCES_FOLDER "lsdsng/happy_birthday.lsdsng", nullptr);
		assert(project != nullptr);

		lsdj_sav_set_project_move(sav, 0, project);

		std::array<unsigned char, 131072> memory;
        size_t writeCount = 0;
		REQUIRE( lsdj_sav_write_to_memory(sav, memory.data(), memory.size(), &writeCount, nullptr) == true );
		REQUIRE( writeCount == LSDJ_SAV_SIZE );

		lsdj_sav_free(sav);

		auto compSav = lsdj_sav_read_from_memory(memory.data(), writeCount, nullptr);
		REQUIRE( compSav != nullptr );
        
        // --- Comparison --- //
        
        // Working memory
        auto compWm = lsdj_sav_get_working_memory_song(compSav);
        REQUIRE( memcmp(compWm->bytes, raw.data(), LSDJ_SONG_BUFFER_BYTE_COUNT) == 0 );

        // Active project
        REQUIRE( lsdj_sav_get_active_project_index(compSav) == LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX );

        auto compProject = lsdj_sav_get_project(compSav, 0);
        REQUIRE( compProject != nullptr );

        std::array<char, LSDJ_PROJECT_NAME_LENGTH> name;
        lsdj_project_get_name(compProject, name.data());
        REQUIRE( strncmp(name.data(), "HAPPY BD", LSDJ_PROJECT_NAME_LENGTH) == 0 );

        REQUIRE( lsdj_project_get_version(compProject) == 4 );

        auto song_buffer = lsdj_project_get_song_buffer(compProject);
        REQUIRE( memcmp(song_buffer->bytes, raw.data(), raw.size()) == 0 );

		lsdj_sav_free(compSav);
	}

	SECTION( "Checking sav likelihood" )
	{
        const auto lsdsng = readFileContents(RESOURCES_FOLDER "lsdsng/happy_birthday.lsdsng");
        assert(lsdsng.size() == 3081);
        
		REQUIRE( lsdj_sav_is_likely_valid_memory(save.data(), save.size(), nullptr) == true );
        REQUIRE( lsdj_sav_is_likely_valid_memory(lsdsng.data(), lsdsng.size(), nullptr) == false );
        REQUIRE( lsdj_sav_is_likely_valid_memory(raw.data(), raw.size(), nullptr) == false );
        
        REQUIRE( lsdj_sav_is_likely_valid_file(RESOURCES_FOLDER "sav/happy_birthday.sav", nullptr) == true );
        REQUIRE( lsdj_sav_is_likely_valid_file(RESOURCES_FOLDER "lsdsng/happy_birthday.lsdsng", nullptr) == false );
        REQUIRE( lsdj_sav_is_likely_valid_file(RESOURCES_FOLDER "raw/happy_birthday.raw", nullptr) == false );
	}
}
