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
        [[nodiscard]] bool shouldProcessSav(const std::filesystem::path& path) const final;
        [[nodiscard]] bool shouldProcessLsdsng(const std::filesystem::path& path) const final;
        
        [[nodiscard]] std::filesystem::path constructSavDestinationPath(const std::filesystem::path& path) final;
        [[nodiscard]] std::filesystem::path constructLsdsngDestinationPath(const std::filesystem::path& path) final;
        
        bool processSong(lsdj_song_t* song) final;
    };
}
