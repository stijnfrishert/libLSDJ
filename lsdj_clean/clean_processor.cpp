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
        // Replace the table in instruments
        for (int i = 0; i < LSDJ_INSTRUMENT_COUNT; i += 1)
        {
            if (lsdj_instrument_t* instrument = lsdj_song_get_instrument(&song, i); instrument)
            {
                replaceTable(*instrument, table, replacement);
            }
        }
        
        // Replace the table in phrases
        for (int p = 0; p < LSDJ_PHRASE_COUNT; p += 1)
        {
            if (lsdj_phrase_t* phrase = lsdj_song_get_phrase(&song, p); phrase)
            {
                replaceTable(*phrase, table, replacement);
            }
        }
    }

    void CleanProcessor::replaceTable(lsdj_instrument_t& instrument, int table, int replacement)
    {
        assert(table != LSDJ_INSTRUMENT_NO_TABLE);
        
        if (lsdj_instrument_get_table(&instrument) == table)
            lsdj_instrument_set_table(&instrument, replacement);
    }

    void CleanProcessor::replaceTable(lsdj_phrase_t& phrase, int table, int replacement)
    {
        for (int i  = 0; i < LSDJ_PHRASE_LENGTH; i++)
        {
            replaceTable(phrase.commands[i], table, replacement);
        }
    }

    void CleanProcessor::replaceTable(lsdj_command_t& command, int table, int replacement)
    {
        if (command.command == LSDJ_COMMAND_A && command.value == table)
            command.value = replacement;
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
                        replacePhrase(song, p2, p1);
                    }
                }
            }
        }
    }

    void CleanProcessor::replacePhrase(lsdj_song_t& song, int phrase, int replacement)
    {
        for (int c = 0; c < LSDJ_CHAIN_COUNT; c += 1)
        {
            if (lsdj_chain_t* chain = lsdj_song_get_chain(&song, c); chain)
                replacePhrase(*chain, phrase, replacement);
        }
    }

    void CleanProcessor::replacePhrase(lsdj_chain_t& chain, int phrase, int replacement)
    {
        assert(phrase != LSDJ_CHAIN_NO_PHRASE);
        
        for (int p = 0; p < LSDJ_CHAIN_LENGTH; p += 1)
        {
            if (chain.phrases[p] == phrase)
                chain.phrases[p] = replacement;
        }
    }
}
