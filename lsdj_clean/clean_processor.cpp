#include "clean_processor.hpp"

#include <assert.h>

namespace lsdj
{
    bool CleanProcessor::processSong(lsdj_song_t& song)
    {
        if (processTables)
            lsdj_song_deduplicate_tables(&song);
        
        if (processPhrases)
        {
            lsdj_song_deduplicate_phrases(&song);
            lsdj_song_deduplicate_chains(&song);
        }
        
        return true;
    }
}
