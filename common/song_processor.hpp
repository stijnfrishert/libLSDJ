#pragma once

#include <vector>

#include <boost/filesystem/path.hpp>

#include "../liblsdj/song.h"

namespace lsdj
{
    class SongProcessor
    {
    public:
        bool process(const boost::filesystem::path& path);
        
    public:
        bool verbose = false;
        
    private:
        bool processDirectory(const boost::filesystem::path& path);
        bool processSav(const boost::filesystem::path& path);
        bool processLsdsng(const boost::filesystem::path& path);
        
        [[nodiscard]] virtual bool shouldProcessSav(const boost::filesystem::path& path) const { return true; }
        [[nodiscard]] virtual bool shouldProcessLsdsng(const boost::filesystem::path& path) const { return true; }
        
        [[nodiscard]] virtual boost::filesystem::path constructSavDestinationPath(const boost::filesystem::path& path) { return path; }
        [[nodiscard]] virtual boost::filesystem::path constructLsdsngDestinationPath(const boost::filesystem::path& path) { return path; }
        
        virtual bool processSong(lsdj_song_t& song) = 0;
    };
}
