#pragma once

#include "../common/song_processor.hpp"

namespace lsdj
{
    class CleanProcessor :
        public SongProcessor
    {
    public:
        bool processInstruments = false;
        bool processTables = false;
        bool processPhrases = false;
        
    private:
        bool processSong(lsdj_song_t* song) final;
        
        void deduplicatePhrases(lsdj_song_t* song);
        void replacePhrase(lsdj_song_t* song, int index, int replacement);
        void replacePhrase(lsdj_chain_t* chain, int index, int replacement);
    };
}
