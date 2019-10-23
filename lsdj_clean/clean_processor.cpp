#include "clean_processor.hpp"

#include <assert.h>

namespace lsdj
{
    bool CleanProcessor::processSong(lsdj_song_t& song)
    {
        if (processTables)
            deduplicateTables(song);
        
        if (processPhrases)
            deduplicatePhrases(song);
        
        return true;
    }

    void CleanProcessor::deduplicateTables(lsdj_song_t& song)
    {
        // For every table number...
        for (int t1 = 0; t1 < LSDJ_TABLE_COUNT; t1 += 1)
        {
            // Check if it's in use
            if (lsdj_table_t* table1 = lsdj_song_get_table(&song, t1); table1)
            {
                // Go through tables higher in number than this one and see if it's a duplicate
                for (int t2 = t1 + 1; t2 < LSDJ_TABLE_COUNT; t2 += 1)
                {
                    if (lsdj_table_t* table2 = lsdj_song_get_table(&song, t2);
                        table2 && lsdj_table_equals(table1, table2))
                    {
                        replaceTable(song, t2, t1);
                    }
                }
            }
        }
    }

    void CleanProcessor::replaceTable(lsdj_song_t& song, int table, int replacement)
    {
        // Replace the table in tables
        for (int i = 0; i < LSDJ_TABLE_COUNT; i += 1)
        {
            if (lsdj_table_t* ptr = lsdj_song_get_table(&song, i); ptr)
            {
                lsdj_table_replace_command_value(ptr, LSDJ_COMMAND_A, table, replacement);
            }
        }
        
        // Replace the table in instruments
        for (int i = 0; i < LSDJ_INSTRUMENT_COUNT; i += 1)
        {
            if (lsdj_instrument_t* instrument = lsdj_song_get_instrument(&song, i); instrument)
            {
                lsdj_instrument_replace_table(instrument, table, replacement);
            }
        }
        
        // Replace the table in phrases
        for (int p = 0; p < LSDJ_PHRASE_COUNT; p += 1)
        {
            if (lsdj_phrase_t* phrase = lsdj_song_get_phrase(&song, p); phrase)
            {
                lsdj_phrase_replace_command_value(phrase, LSDJ_COMMAND_A, table, replacement);
            }
        }
    }

    void CleanProcessor::deduplicatePhrases(lsdj_song_t& song)
    {
        // For every phrase number...
        for (int p1 = 0; p1 < LSDJ_PHRASE_COUNT; p1 += 1)
        {
            // Check if it's in use
            if (lsdj_phrase_t* phrase1 = lsdj_song_get_phrase(&song, p1); phrase1)
            {
                // Go through phrases higher than this one and see if it's a duplicate
                for (int p2 = p1 + 1; p2 < LSDJ_PHRASE_COUNT; p2 += 1)
                {
                    if (lsdj_phrase_t* phrase2 = lsdj_song_get_phrase(&song, p2);
                        phrase2 && lsdj_phrase_equals(phrase1, phrase2))
                    {
                        lsdj_song_replace_phrase(&song, p2, p1);
                    }
                }
            }
        }
    }
}
