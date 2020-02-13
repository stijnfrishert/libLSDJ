#include <song.h>

#include <array>
#include <catch2/catch.hpp>
#include <chain.h>
#include <command.h>
#include <groove.h>
#include <instrument.h>
#include <panning.h>
#include <phrase.h>
#include <sav.h>
#include <synth.h>
#include <table.h>
#include <wave.h>

using namespace Catch;

TEST_CASE( "Song", "[song]" )
{
	auto sav = lsdj_sav_read_from_file(RESOURCES_FOLDER "sav/all.sav", nullptr);
	REQUIRE( sav != nullptr );

	REQUIRE(LSDJ_SONG_BYTE_COUNT == 0x8000);

	SECTION( "Happy Birthday" )
	{
		auto song = lsdj_project_get_song(lsdj_sav_get_project(sav, 0));
		assert(song != nullptr);
        
		SECTION( "Song settings" )
		{
			REQUIRE( lsdj_song_get_format_version(song) == 7 );
			REQUIRE( lsdj_song_has_changed(song) == false );
			REQUIRE( lsdj_song_get_tempo(song) == 88 );
			REQUIRE( lsdj_song_get_transposition(song) == 0 );
			REQUIRE( lsdj_song_get_sync_mode(song) == LSDJ_SYNC_NONE );
		}

		SECTION( "Editor settings" )
		{
			REQUIRE( lsdj_song_get_clone_mode(song) == LSDJ_CLONE_DEEP );
			REQUIRE( lsdj_song_get_font(song) == 0 );
			REQUIRE( lsdj_song_get_color_palette(song) == 0 );
			REQUIRE( lsdj_song_get_key_delay(song) == 7 );
			REQUIRE( lsdj_song_get_key_repeat(song) == 2 );
			REQUIRE( lsdj_song_get_prelisten(song) == true );
		}

		SECTION( "Clock" )
		{
			REQUIRE( lsdj_song_get_total_days(song) == 0);
			REQUIRE( lsdj_song_get_total_hours(song) == 18);
			REQUIRE( lsdj_song_get_total_minutes(song) == 10);

			REQUIRE( lsdj_song_get_work_hours(song) == 2);
			REQUIRE( lsdj_song_get_work_minutes(song) == 4);
		}

		SECTION( "Rows" )
		{
			REQUIRE( lsdj_row_get_chain(song, 0, LSDJ_CHANNEL_PULSE1) == 1 );
			REQUIRE( lsdj_row_get_chain(song, 0, LSDJ_CHANNEL_PULSE2) == 2 );
			REQUIRE( lsdj_row_get_chain(song, 0, LSDJ_CHANNEL_WAVE) == 3 );
			REQUIRE( lsdj_row_get_chain(song, 0, LSDJ_CHANNEL_NOISE) == 4 );
			REQUIRE( lsdj_row_get_chain(song, 1, LSDJ_CHANNEL_PULSE1) == LSDJ_SONG_NO_CHAIN );
            
            REQUIRE( lsdj_song_is_row_bookmarked(song, 0, LSDJ_CHANNEL_PULSE1) == false );
            REQUIRE( lsdj_song_is_row_bookmarked(song, 2, LSDJ_CHANNEL_PULSE2) == false );
            REQUIRE( lsdj_song_is_row_bookmarked(song, 1, LSDJ_CHANNEL_WAVE) == false );
            REQUIRE( lsdj_song_is_row_bookmarked(song, 3, LSDJ_CHANNEL_NOISE) == false );
            REQUIRE( lsdj_song_is_row_bookmarked(song, 5, LSDJ_CHANNEL_PULSE1) == false );
		}

		SECTION( "Chains" )
		{
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
			REQUIRE( lsdj_chain_get_phrase(song, 0x05, 0) == LSDJ_CHAIN_NO_PHRASE );

			REQUIRE( lsdj_chain_get_transposition(song, 0x03, 15) == 0 );
		}

		SECTION( "Phrases" )
		{
			REQUIRE( lsdj_phrase_is_allocated(song, 0x00) == false );
			REQUIRE( lsdj_phrase_is_allocated(song, 0x01) == true );
			REQUIRE( lsdj_phrase_is_allocated(song, 0x0A) == true );
			REQUIRE( lsdj_phrase_is_allocated(song, 0x1A) == true );
			REQUIRE( lsdj_phrase_is_allocated(song, 0x24) == true );
			REQUIRE( lsdj_phrase_is_allocated(song, 0x26) == false );

			REQUIRE( lsdj_phrase_get_note(song, 0x05, 0x3) == 49 );
			REQUIRE( lsdj_phrase_get_note(song, 0x18, 0xE) == 19 );
			REQUIRE( lsdj_phrase_get_note(song, 0x1C, 0x0) == 16 );
			REQUIRE( lsdj_phrase_get_note(song, 0x08, 0x0B) == LSDJ_PHRASE_NO_NOTE );

			REQUIRE( lsdj_phrase_get_instrument(song, 0x0C, 0x6) == 0x02 );
			REQUIRE( lsdj_phrase_get_instrument(song, 0x16, 0x0) == 0x03 );
			REQUIRE( lsdj_phrase_get_instrument(song, 0x16, 0x4) == 0x06 );
			REQUIRE( lsdj_phrase_get_instrument(song, 0x16, 0x6) == LSDJ_PHRASE_NO_INSTRUMENT );

			REQUIRE( lsdj_phrase_get_command(song, 0x12, 0x2) == LSDJ_COMMAND_L );
			REQUIRE( lsdj_phrase_get_command_value(song, 0x12, 0x2) == 0x20 );
			REQUIRE( lsdj_phrase_get_command(song, 0x0E, 0xA) == LSDJ_COMMAND_P );
			REQUIRE( lsdj_phrase_get_command_value(song, 0x0E, 0xA) == 0x0F );
			REQUIRE( lsdj_phrase_get_command(song, 0x05, 0x3) == LSDJ_COMMAND_E );
			REQUIRE( lsdj_phrase_get_command_value(song, 0x05, 0x3) == 0xA3 );
			REQUIRE( lsdj_phrase_get_command(song, 0x16, 0x0) == LSDJ_COMMAND_NONE );
		}

		SECTION( "Instruments" )
		{
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
		}

		SECTION( "Synths" )
		{
			REQUIRE( lsdj_synth_get_waveform(song, 0x0) == LSDJ_SYNTH_WAVEFORM_TRIANGLE );
			REQUIRE( lsdj_synth_get_waveform(song, 0x1) == LSDJ_SYNTH_WAVEFORM_SAW );
			REQUIRE( lsdj_synth_get_distortion(song, 0x0) == LSDJ_SYNTH_DISTORTION_CLIP );
			REQUIRE( lsdj_synth_get_phase_compression(song, 0x0) == LSDJ_SYNTH_PHASE_NORMAL );

			REQUIRE( lsdj_synth_get_volume_start(song, 0x0) == 0x30 );
			REQUIRE( lsdj_synth_get_volume_end(song, 0x0) == 0x10 );
			REQUIRE( lsdj_synth_get_resonance_start(song, 0x0) == 0x0 );
			REQUIRE( lsdj_synth_get_resonance_end(song, 0x0) == 0x0 );
			REQUIRE( lsdj_synth_get_cutoff_start(song, 0x0) == 0xFF );
			REQUIRE( lsdj_synth_get_cutoff_end(song, 0x0) == 0xFF );
			REQUIRE( lsdj_synth_get_vshift_start(song, 0x0) == 0x0 );
			REQUIRE( lsdj_synth_get_vshift_end(song, 0x0) == 0x0 );
			REQUIRE( lsdj_synth_get_limit_start(song, 0x0) == 0xF );
			REQUIRE( lsdj_synth_get_limit_end(song, 0x0) == 0xF );
			REQUIRE( lsdj_synth_get_phase_start(song, 0x0) == 0x00 );
			REQUIRE( lsdj_synth_get_phase_end(song, 0x0) == 0x00 );
		}

		SECTION( "Waves" )
		{
			std::array<std::uint8_t, LSDJ_WAVE_BYTE_COUNT> wave00 = {
				0x89, 0xBD, 0xFF, 0xDF, 0xFF, 0xFF, 0xFD, 0xB9, 0x86, 0x42, 0x00, 0x00, 0x00, 0x00, 0x02, 0x46
			};

			REQUIRE( memcmp(lsdj_wave_get_bytes(song, 0x00), wave00.data(), wave00.size()) == 0 );

			std::array<std::uint8_t, LSDJ_WAVE_BYTE_COUNT> wave31 = {
				0x8E, 0xCD, 0xCC, 0xBB, 0xAA, 0xA9, 0x99, 0x88, 0x87, 0x76, 0x66, 0x55, 0x54, 0x43, 0x32, 0x31
			};

			REQUIRE( memcmp(lsdj_wave_get_bytes(song, 0x31), wave31.data(), wave31.size()) == 0 );
		}

		SECTION( "Tables" )
		{
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

		SECTION(" Grooves" )
		{
			REQUIRE( lsdj_groove_get_step(song, 0, 0) == 6 );
			REQUIRE( lsdj_groove_get_step(song, 0, 1) == 6 );
			REQUIRE( lsdj_groove_get_step(song, 0, 2) == LSDJ_GROOVE_NO_VALUE );
		}
	}
}
