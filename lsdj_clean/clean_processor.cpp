#include "clean_processor.hpp"

namespace lsdj
{
    bool CleanProcessor::processSong(lsdj_song_t* song)
    {
        deduplicatePhrases(song);
        
        return true;
    }

    void CleanProcessor::deduplicatePhrases(lsdj_song_t* song)
    {
        // For every phrase number...
        for (int p1 = 0; p1 < LSDJ_PHRASE_COUNT; p1 += 1)
        {
            // Check if it's in use
            lsdj_phrase_t* phrase1 = lsdj_song_get_phrase(song, p1);
            if (!phrase1)
                continue;
            
            // Go through phrases higher than this one and see if it's a duplicate
            for (int p2 = p1 + 1; p2 < LSDJ_PHRASE_COUNT; p2 += 1)
            {
                lsdj_phrase_t* phrase2 = lsdj_song_get_phrase(song, p2);
                if (!phrase2)
                    continue;
                
                if (lsdj_phrase_equals(phrase1, phrase2))
                {
                    replacePhrase(song, p2, p1);
                }
            }
        }
    }

    void CleanProcessor::replacePhrase(lsdj_song_t* song, int index, int replacement)
    {
        for (int r = 0; r < LSDJ_ROW_COUNT; r += 1)
        {
            lsdj_row_t* row = lsdj_song_get_row(song, r);
            
            if (lsdj_chain_t* chain = lsdj_song_get_chain(song, row->pulse1); chain)
                replacePhrase(chain, index, replacement);
            
            if (lsdj_chain_t* chain = lsdj_song_get_chain(song, row->pulse2); chain)
                replacePhrase(chain, index, replacement);
            
            if (lsdj_chain_t* chain = lsdj_song_get_chain(song, row->wave); chain)
                replacePhrase(chain, index, replacement);
            
            if (lsdj_chain_t* chain = lsdj_song_get_chain(song, row->noise); chain)
                replacePhrase(chain, index, replacement);
        }
    }

    void CleanProcessor::replacePhrase(lsdj_chain_t* chain, int index, int replacement)
    {
        for (int p = 0; p < LSDJ_CHAIN_LENGTH; p += 1)
        {
            if (chain->phrases[p] == index)
                chain->phrases[p] = replacement;
        }
    }
}
