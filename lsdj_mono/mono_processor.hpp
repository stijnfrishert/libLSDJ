#pragma once

#include "../common/song_processor.hpp"

namespace lsdj
{
    class MonoProcessor :
        public SongProcessor
    {
    public:
        bool processInstruments = false;
        bool processTables = false;
        bool processPhrases = false;
        
    private:
        [[nodiscard]] bool shouldProcessSav(const boost::filesystem::path& path) const final;
        [[nodiscard]] bool shouldProcessLsdsng(const boost::filesystem::path& path) const final;
        
        [[nodiscard]] boost::filesystem::path constructSavDestinationPath(const boost::filesystem::path& path) final;
        [[nodiscard]] boost::filesystem::path constructLsdsngDestinationPath(const boost::filesystem::path& path) final;
        
        bool processSong(lsdj_song_t* song) final;
    };
}
