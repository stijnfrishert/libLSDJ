/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 Stijn Frishert
 
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

#include <array>
#include <boost/filesystem.hpp>
#include <iostream>

#include "../liblsdj/error.h"
#include "../liblsdj/sav.h"
#include "../liblsdj/song.h"
#include "../liblsdj/project.h"

#include "../common/common.hpp"
#include "importer.hpp"

namespace lsdj
{
    int Importer::importSongs(const std::vector<std::string>& inputs, std::string outputFile, const char* savName)
    {
        lsdj_error_t* error = nullptr;
        lsdj_sav_t* sav = savName ? lsdj_sav_read_from_file(boost::filesystem::absolute(savName).string().c_str(), &error) : lsdj_sav_new(&error);
        if (error)
            return handle_error(error);
        
        auto index = 0;
        for( ; index < lsdj_sav_get_project_count(sav); ++index)
        {
            lsdj_project_t* project = lsdj_sav_get_project(sav, index);
            lsdj_song_t* song = lsdj_project_get_song(project);
            if (!song)
                break;
        }
        
        if (savName)
            std::cout << "Read " << savName << ", containing " << std::to_string(index) << " saves" << std::endl;
        
        std::vector<boost::filesystem::path> paths;
        boost::filesystem::path workingMemoryPath;
        for (auto& input : inputs)
        {
            const auto path = boost::filesystem::absolute(input);
            if (isHiddenFile(path.filename().string()))
                continue;
            
            if (boost::filesystem::is_regular_file(path))
            {
                paths.emplace_back(path);
            }
            else if (boost::filesystem::is_directory(path))
            {
                std::vector<boost::filesystem::path> contents;
                for (auto it = boost::filesystem::directory_iterator(path); it != boost::filesystem::directory_iterator(); ++it)
                {
                    const auto path = it->path();
                    if (isHiddenFile(path.filename().string()) || !boost::filesystem::is_regular_file(path) || path.extension() != ".lsdsng")
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
            } else {
                throw std::runtime_error(path.string() + " is not a file or directory");
            }
        }
        
        const auto active = lsdj_sav_get_active_project(sav);
        for (auto i = 0; i < paths.size(); ++i)
        {
            if (index == lsdj_sav_get_project_count(sav))
                break;
            
            lsdj_project_t* project = lsdj_project_read_lsdsng_from_file(paths[i].string().c_str(), &error);
            if (error)
            {
                lsdj_sav_free(sav);
                return handle_error(error);
            }
            
            lsdj_sav_set_project(sav, index, project, &error);
            if (error)
            {
                lsdj_project_free(project);
                lsdj_sav_free(sav);
                return handle_error(error);
            }
            
            std::array<char, 9> name;
            name.fill('\0');
            lsdj_project_get_name(project, name.data(), 8);
            
            if (outputFile.empty() && inputs.size() == 1)
                outputFile = std::string(name.data()) + ".sav";
            
            if (verbose)
                std::cout << "Imported " << name.data() << " at slot " << std::to_string(index) << std::endl;
            
            if (i == 0 && active == LSDJ_NO_ACTIVE_PROJECT && workingMemoryPath.empty())
            {
                lsdj_sav_set_working_memory_song_from_project(sav, i, &error);
                if (error)
                {
                    lsdj_project_free(project);
                    lsdj_sav_free(sav);
                    return handle_error(error);
                }
            }
            
            index += 1;
        }
        
        if (!workingMemoryPath.empty())
        {
            lsdj_project_t* project = lsdj_project_read_lsdsng_from_file(workingMemoryPath.string().c_str(), &error);
            if (error)
            {
                lsdj_project_free(project);
                lsdj_sav_free(sav);
                return handle_error(error);
            }
            
            lsdj_song_t* song = lsdj_song_copy(lsdj_project_get_song(project), &error);
            if (error)
            {
                lsdj_project_free(project);
                lsdj_sav_free(sav);
                return handle_error(error);
            }
            
            int active = LSDJ_NO_ACTIVE_PROJECT;
            const auto str = workingMemoryPath.stem().string();
            const auto stem = str.substr(0, str.size() - 3);
            for (int i = 0; i != paths.size(); ++i)
            {
                if (stem == paths[i].stem().string())
                {
                    active = i;
                    break;
                }
            }
            
            lsdj_sav_set_working_memory_song(sav, song, active);
            
            lsdj_project_free(project);
        }
        
        if (outputFile.empty())
            outputFile = "out.sav";
        
        lsdj_sav_write_to_file(sav, boost::filesystem::absolute(outputFile).string().c_str(), &error);
        if (error)
        {
            lsdj_sav_free(sav);
            return handle_error(error);
        }
        
        return 0;
    }
}
