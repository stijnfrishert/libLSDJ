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
        bool processSong(lsdj_song_t& song) final;
        
        void deduplicateTables(lsdj_song_t& song);
        void replaceTable(lsdj_song_t& song, int table, int replacement);
        void replaceTable(lsdj_instrument_t& instrument, int table, int replacement);
        void replaceTable(lsdj_phrase_t& phrase, int table, int replacement);
        void replaceTable(lsdj_command_t& command, int table, int replacement);
        
        void deduplicatePhrases(lsdj_song_t& song);
        void replacePhrase(lsdj_song_t& song, int phrase, int replacement);
        void replacePhrase(lsdj_chain_t& chain, int phrase, int replacement);
    };
}
