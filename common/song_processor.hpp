#pragma once

#include <filesystem>
#include <vector>

#include <lsdj/song.h>

namespace lsdj
{
    class SongProcessor
    {
    public:
        bool process(const std::filesystem::path& path);
        
    public:
        bool verbose = false;
        
    private:
        bool processDirectory(const std::filesystem::path& path);
        bool processSav(const std::filesystem::path& path);
        bool processLsdsng(const std::filesystem::path& path);
        
        [[nodiscard]] virtual bool shouldProcessSav(const std::filesystem::path& path) const { return true; }
        [[nodiscard]] virtual bool shouldProcessLsdsng(const std::filesystem::path& path) const { return true; }
        
        [[nodiscard]] virtual std::filesystem::path constructSavDestinationPath(const std::filesystem::path& path) { return path; }
        [[nodiscard]] virtual std::filesystem::path constructLsdsngDestinationPath(const std::filesystem::path& path) { return path; }
        
        virtual bool processSong(lsdj_song_t* song) = 0;
    };
}
