/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 - 2020 Stijn Frishert
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 */

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

#include <lsdj/song.h>
#include <lsdj/project.h>

#include "../common/common.hpp"
#include "importer.hpp"

namespace lsdj
{
    // Scan a path, see whether it's either an .lsdsng or a folder containing .lsdsng's
    // Returns the path to the .WM (working memory) file, or {} is there was none
    ghc::filesystem::path scanPath(const ghc::filesystem::path& path, std::vector<ghc::filesystem::path>& paths)
    {
        if (isHiddenFile(path.filename().string()))
            return {};
        
        if (ghc::filesystem::is_regular_file(path))
        {
            paths.emplace_back(path);
            return {};
        }
        else if (ghc::filesystem::is_directory(path))
        {
            ghc::filesystem::path workingMemoryPath;
            std::vector<ghc::filesystem::path> contents;
            for (auto it = ghc::filesystem::directory_iterator(path); it != ghc::filesystem::directory_iterator(); ++it)
            {
                const auto path = it->path();
                if (isHiddenFile(path.filename().string()) || !ghc::filesystem::is_regular_file(path) || path.extension() != ".lsdsng" || path.extension() != ".sav")
                    continue;
                
                const auto str = path.stem().string();
                if (str.size() >= 3 && str.substr(str.length() - 3) == ".WM")
                {
                    workingMemoryPath = path;
                    continue;
                }
                
                contents.emplace_back(path);
            }
            
            std::sort(contents.begin(), contents.end());
            for (auto& path : contents)
                paths.emplace_back(path);
            
            return workingMemoryPath;
        } else {
            throw std::runtime_error(path.string() + " is not a file or directory");
        }
    }
    
    int Importer::import()
    {
        // Create an empty sav to work with
        lsdj_sav_t* sav = nullptr;
        lsdj_error_t error = lsdj_sav_new(&sav, nullptr);
        if (error != LSDJ_SUCCESS)
            return handle_error(error);
        assert(sav != nullptr);
        
        // Find the first available project slot index
        uint8_t index = 0;
        for( ; index < LSDJ_SAV_PROJECT_COUNT; ++index)
        {
            if (!lsdj_sav_get_project_const(sav, index))
                break;
        }
        
        // Go through all input files and recursively find all .lsdsngs's (and the working memory file)
        std::vector<ghc::filesystem::path> paths;
        for (auto& input : inputs)
        {
            const auto wm = scanPath(ghc::filesystem::absolute(input), paths);
            if (!wm.empty())
            {
                if (!workingMemoryPath.empty())
                {
                    std::cerr << "Multiple working memory (.WM) .lsdsng's found" << std::endl;
                    return 1;
                }
                
                workingMemoryPath = wm;
            }
        }
        
        assert(!outputFile.empty());
        
        // Import all lsdsng/sav files
        const auto active = lsdj_sav_get_active_project_index(sav);
        for (auto i = 0; i < paths.size(); ++i)
        {
            if (index == LSDJ_SAV_PROJECT_COUNT)
            {
                std::cerr << "Reached maximum project count, can't write " << paths[i].string() << std::endl;
                break;
            }
            
            const lsdj_error_t error = import(paths[i], sav, index, active);
            if (error != LSDJ_SUCCESS)
            {
                lsdj_sav_free(sav);
                return handle_error(error);
            }
        }
        
        if (!workingMemoryPath.empty())
        {
            const lsdj_error_t error = importWorkingMemorySong(sav, paths);
            if (error != LSDJ_SUCCESS)
            {
                lsdj_sav_free(sav);
                return handle_error(error);
            }
        }
        
        // Write the sav to file
        error = lsdj_sav_write_to_file(sav, ghc::filesystem::absolute(outputFile).string().c_str(), nullptr);
        if (error != LSDJ_SUCCESS)
        {
            lsdj_sav_free(sav);
            return handle_error(error);
        }
        
        return 0;
    }

    lsdj_error_t Importer::import(const ghc::filesystem::path& path, lsdj_sav_t* sav, uint8_t& index, uint8_t active)
    {
        if (path.extension() == ".sav")
            return importSav(path.string(), sav, index, active);
        else if (path.extension() == ".lsdsng")
            return importSong(path.string(), sav, index, active);
    }

    lsdj_error_t Importer::importSav(const std::string& path, lsdj_sav_t* destSav, uint8_t& index, uint8_t active)
    {
        lsdj_sav_t* sourceSav = nullptr;
        lsdj_error_t result = lsdj_sav_read_from_file(path.data(), &sourceSav, nullptr);
        if (result != LSDJ_SUCCESS)
            return result;
        
        for (uint8_t i = 0; i < LSDJ_SAV_PROJECT_COUNT && index < LSDJ_SAV_PROJECT_COUNT; ++i)
        {
            const lsdj_project_t* project = lsdj_sav_get_project_const(sourceSav, i);
            if (project)
            {
                result = importProject(project, destSav, index, active);
                if (result != LSDJ_SUCCESS)
                    break;
            }
        }
        
        lsdj_sav_free(sourceSav);
        
        return LSDJ_SUCCESS;
    }
    
    lsdj_error_t Importer::importSong(const std::string& path, lsdj_sav_t* sav, uint8_t& index, uint8_t active)
    {
        lsdj_project_t* project = nullptr;
        lsdj_error_t error = lsdj_project_read_lsdsng_from_file(path.c_str(), &project, nullptr);
        if (error == LSDJ_SUCCESS)
        {
            error = importProject(project, sav, index, active);
            lsdj_project_free(project);
            
            if (error != LSDJ_SUCCESS)
                return error;
        }
        
        return LSDJ_SUCCESS;
    }

    lsdj_error_t Importer::importProject(const lsdj_project_t* project, lsdj_sav_t* sav, uint8_t& index, uint8_t active)
    {
        assert(project != nullptr);
        
        lsdj_error_t error = lsdj_sav_set_project_copy(sav, index, project, nullptr);
        if (error != LSDJ_SUCCESS)
            return error;
        
        if (verbose)
        {
            const auto n = lsdj_project_get_name(project);
            std::string name(n, strnlen(n, LSDJ_PROJECT_NAME_LENGTH));
            std::cout << "Imported " << name.data() << " at slot " << std::to_string(index) << std::endl;
        }
        
        const auto oldIndex = index;
        index += 1;
        
        if (oldIndex == 0 && active == LSDJ_SAV_NO_ACTIVE_PROJECT_INDEX && workingMemoryPath.empty())
        {
            lsdj_error_t error = lsdj_sav_set_working_memory_song_from_project(sav, oldIndex);
            if (error != LSDJ_SUCCESS)
                return error;
        }
        
        return LSDJ_SUCCESS;
    }
    
    lsdj_error_t Importer::importWorkingMemorySong(lsdj_sav_t* sav, const std::vector<ghc::filesystem::path>& paths)
    {
        lsdj_project_t* project = nullptr;
        lsdj_error_t error = lsdj_project_read_lsdsng_from_file(workingMemoryPath.string().c_str(), &project, nullptr);
        if (error != LSDJ_SUCCESS)
            return error;
        assert(project != nullptr);
        
        const auto song = lsdj_project_get_song_const(project);
        lsdj_sav_set_working_memory_song(sav, song);
        
        // Find out if one of the slots has the same name as the working memory filename
        const auto str = workingMemoryPath.stem().string();
        const auto stem = str.substr(0, str.size() - 3);
        for (int i = 0; i != paths.size(); ++i)
        {
            if (stem == paths[i].stem().string())
            {
                lsdj_sav_set_active_project_index(sav, i);
                break;
            }
        }
        
        lsdj_project_free(project);
        
        return LSDJ_SUCCESS;
    }
}
