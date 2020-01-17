#pragma once

#include <vector>

#include <ghc/filesystem.hpp>

#include "../liblsdj/song.h"

namespace lsdj
{
    class SongProcessor
    {
    public:
        bool process(const ghc::filesystem::path& path);
        
    public:
        bool verbose = false;
        
    private:
        bool processDirectory(const ghc::filesystem::path& path);
        bool processSav(const ghc::filesystem::path& path);
        bool processLsdsng(const ghc::filesystem::path& path);
        
        [[nodiscard]] virtual bool shouldProcessSav(const ghc::filesystem::path& path) const { return true; }
        [[nodiscard]] virtual bool shouldProcessLsdsng(const ghc::filesystem::path& path) const { return true; }
        
        [[nodiscard]] virtual ghc::filesystem::path constructSavDestinationPath(const ghc::filesystem::path& path) { return path; }
        [[nodiscard]] virtual ghc::filesystem::path constructLsdsngDestinationPath(const ghc::filesystem::path& path) { return path; }
        
        virtual bool processSong(lsdj_song_t& song) = 0;
    };
}
