#include <song.h>

#include <catch2/catch.hpp>
#include <command.h>
#include <instrument.h>
#include <panning.h>
#include <sav.h>
#include <table.h>

using namespace Catch;

SCENARIO( "Song", "[song]" )
{
	auto sav = lsdj_sav_read_from_file(RESOURCES_FOLDER "sav/all.sav", nullptr);
	REQUIRE( sav != nullptr );

	REQUIRE(LSDJ_SONG_BYTE_COUNT == 0x8000);

	GIVEN( "Happy Birthday" )
	{
		auto song = lsdj_project_get_song(lsdj_sav_get_project(sav, 0));
		assert(song != nullptr);

		REQUIRE( lsdj_song_get_format_version(song) == 7 );
		REQUIRE( lsdj_song_has_changed(song) == false );
		REQUIRE( lsdj_song_get_tempo(song) == 88 );
		REQUIRE( lsdj_song_get_transposition(song) == 0 );
		REQUIRE( lsdj_song_get_sync_mode(song) == LSDJ_SYNC_NONE );

		REQUIRE( lsdj_song_get_clone_mode(song) == LSDJ_CLONE_DEEP );
		REQUIRE( lsdj_song_get_font(song) == 0 );
		REQUIRE( lsdj_song_get_color_palette(song) == 0 );
		REQUIRE( lsdj_song_get_key_delay(song) == 7 );
		REQUIRE( lsdj_song_get_key_repeat(song) == 2 );
		REQUIRE( lsdj_song_get_prelisten(song) == true );

		REQUIRE( lsdj_song_get_total_days(song) == 0);
		REQUIRE( lsdj_song_get_total_hours(song) == 18);
		REQUIRE( lsdj_song_get_total_minutes(song) == 10);

		// --- Rows --- //

		REQUIRE( lsdj_row_get_chain(song, 0, LSDJ_CHANNEL_PULSE1) == 1 );
		REQUIRE( lsdj_row_get_chain(song, 0, LSDJ_CHANNEL_PULSE2) == 2 );
		REQUIRE( lsdj_row_get_chain(song, 0, LSDJ_CHANNEL_WAVE) == 3 );
		REQUIRE( lsdj_row_get_chain(song, 0, LSDJ_CHANNEL_NOISE) == 4 );
		REQUIRE( lsdj_row_get_chain(song, 1, LSDJ_CHANNEL_PULSE1) == LSDJ_NO_CHAIN );

		REQUIRE( lsdj_chain_is_allocated(song, 0x00) == false );
		REQUIRE( lsdj_chain_is_allocated(song, 0x01) == true );
		REQUIRE( lsdj_chain_is_allocated(song, 0x02) == true );
		REQUIRE( lsdj_chain_is_allocated(song, 0x03) == true );
		REQUIRE( lsdj_chain_is_allocated(song, 0x04) == true );
		REQUIRE( lsdj_chain_is_allocated(song, 0x05) == false );

		REQUIRE( lsdj_chain_get_phrase(song, 0x01, 6) == 0x07 );
		REQUIRE( lsdj_chain_get_phrase(song, 0x02, 5) == 0x1B );
		REQUIRE( lsdj_chain_get_phrase(song, 0x03, 2) == 0x0E );
		REQUIRE( lsdj_chain_get_phrase(song, 0x04, 9) == 0x25 );
		REQUIRE( lsdj_chain_get_transposition(song, 0x03, 15) == 0 );

		REQUIRE( lsdj_phrase_is_allocated(song, 0x00) == false );
		REQUIRE( lsdj_phrase_is_allocated(song, 0x01) == true );
		REQUIRE( lsdj_phrase_is_allocated(song, 0x0A) == true );
		REQUIRE( lsdj_phrase_is_allocated(song, 0x1A) == true );
		REQUIRE( lsdj_phrase_is_allocated(song, 0x24) == true );
		REQUIRE( lsdj_phrase_is_allocated(song, 0x26) == false );

		REQUIRE( lsdj_phrase_get_note(song, 0x05, 3) == 49 );

		// --- Instruments --- //

		for (uint8_t i = 0; i < 10; i += 1)
			REQUIRE( lsdj_instrument_is_allocated(song, i) );
		REQUIRE( lsdj_instrument_is_allocated(song, 10) != true );

		REQUIRE( strncmp(lsdj_instrument_get_name(song, 0), "LEAD1", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );
		REQUIRE( strncmp(lsdj_instrument_get_name(song, 1), "LEAD2", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );
		REQUIRE( strncmp(lsdj_instrument_get_name(song, 2), "SIDE", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );
		REQUIRE( strncmp(lsdj_instrument_get_name(song, 3), "KICK", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );
		REQUIRE( strncmp(lsdj_instrument_get_name(song, 4), "HATC", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );
		REQUIRE( strncmp(lsdj_instrument_get_name(song, 5), "SNARE", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );
		REQUIRE( strncmp(lsdj_instrument_get_name(song, 6), "BASS", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );
		REQUIRE( strncmp(lsdj_instrument_get_name(song, 7), "ARP", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );
		REQUIRE( strncmp(lsdj_instrument_get_name(song, 8), "SIDE", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );
		REQUIRE( strncmp(lsdj_instrument_get_name(song, 9), "ARP2", LSDJ_INSTRUMENT_NAME_LENGTH) == 0 );

		// --- Tables --- //

		for (uint8_t i = 0; i < 4; i += 1)
			REQUIRE( lsdj_table_is_allocated(song, i) );
		REQUIRE( lsdj_table_is_allocated(song, 5) != true );

		REQUIRE( lsdj_table_get_envelope(song, 2, 0) == 0x00 );

		REQUIRE( lsdj_table_get_transposition(song, 2, 0) == 0x00 );
		REQUIRE( lsdj_table_get_transposition(song, 2, 1) == 0x02 );
		REQUIRE( lsdj_table_get_transposition(song, 2, 2) == 0x0C );
		REQUIRE( lsdj_table_get_transposition(song, 2, 3) == 0x0E );
		REQUIRE( lsdj_table_get_transposition(song, 2, 4) == 0x10 );

		REQUIRE( lsdj_table_get_command1(song, 0, 0) == LSDJ_COMMAND_P );
		REQUIRE( lsdj_table_get_command1_value(song, 0, 0) == 0xDB );
		REQUIRE( lsdj_table_get_command1(song, 0, 1) == LSDJ_COMMAND_NONE );
		REQUIRE( lsdj_table_get_command1(song, 0, 3) == LSDJ_COMMAND_K );
		REQUIRE( lsdj_table_get_command1_value(song, 0, 3) == 0x00 );

		REQUIRE( lsdj_table_get_command2(song, 1, 0) == LSDJ_COMMAND_O );
		REQUIRE( lsdj_table_get_command2_value(song, 1, 0) == LSDJ_PAN_LEFT );
		REQUIRE( lsdj_table_get_command2(song, 1, 1) == LSDJ_COMMAND_O );
		REQUIRE( lsdj_table_get_command2_value(song, 1, 1) == LSDJ_PAN_LEFT_RIGHT );
		REQUIRE( lsdj_table_get_command2(song, 1, 2) == LSDJ_COMMAND_NONE );
	}
}
