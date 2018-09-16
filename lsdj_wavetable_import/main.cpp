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
#include <cstring>
#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "../common/common.hpp"
#include "../liblsdj/project.h"

int apply(const std::string& projectName, const std::string& wavetableName, unsigned int synthIndex)
{
    const auto projectPath = boost::filesystem::absolute(projectName);
    if (!boost::filesystem::exists(projectPath))
    {
        std::cerr << projectPath.filename().string() << " does not exist" << std::endl;
        return 1;
    }
    
    // Load the project
    lsdj_error_t* error = nullptr;
    lsdj_project_t* project = lsdj_project_read_lsdsng_from_file(projectPath.string().c_str(), &error);
    if (error != nullptr)
    {
        lsdj_project_free(project);
        return 1;
    }
    
    lsdj_song_t* song = lsdj_project_get_song(project);
    
    // Find the wavetable file
    const auto wavetablePath = boost::filesystem::absolute(wavetableName);
    if (!boost::filesystem::exists(wavetablePath))
    {
        std::cerr << wavetablePath.filename().string() << " does not exist" << std::endl;
        lsdj_project_free(project);
        return 1;
    }
    
    // Make sure the wavetable is the correct size
    const auto wavetableSize = boost::filesystem::file_size(wavetablePath);
    if (wavetableSize % 16 != 0)
    {
        std::cerr << "The wavetable file size is not a multiple of 16 bytes" << std::endl;
        lsdj_project_free(project);
        return 1;
    }
    
    if (wavetableSize > 256)
    {
        std::cerr << "The wavetable file size is bigger than 256 bytes" << std::endl;
        lsdj_project_free(project);
        return 1;
    }
    
    // Load the wavetable file
    std::ifstream wavetableStream(wavetablePath.string());
    if (!wavetableStream.is_open())
    {
        std::cerr << "could not open " << wavetablePath.filename().string() << std::endl;
        lsdj_project_free(project);
        return 1;
    }
    
    // Apply the wavetable
    const auto frameCount = wavetableSize / 16;
    std::array<char, LSDJ_WAVE_LENGTH> table;
    for (auto frame = 0; frame < frameCount; frame++)
    {
        wavetableStream.read(table.data(), sizeof(table));
        lsdj_wave_t* wave = lsdj_song_get_wave(song, synthIndex * 16 + frame);
        memcpy(wave->data, table.data(), sizeof(table));
    }
    
    if (false)
    {
        table.fill(0x88);
        for (auto frame = frameCount; frame < 16; frame++)
        {
            wavetableStream.read(table.data(), sizeof(table));
            lsdj_wave_t* wave = lsdj_song_get_wave(song, synthIndex * 16 + frame);
            memcpy(wave->data, table.data(), sizeof(table));
        }
    }
    
    // Write the project back to file
    lsdj_project_write_lsdsng_to_file(project, projectPath.string().c_str(), &error);
    if (error != nullptr)
    {
        lsdj_project_free(project);
        return 1;
    }
    
    std::cout << "Consider it done" << std::endl;
    
    return 0;
}

void printHelp(const boost::program_options::options_description& desc)
{
    std::cout << "lsdj-wavetable-import [project] [wavetable] [index]\n\n" << desc;
}

int main(int argc, char* argv[])
{
    boost::program_options::options_description hidden{"Hidden"};
    hidden.add_options()
        ("project", "The .lsdsng project to which the wavetable should be applied")
        ("wavetable", "The wavetable that is applied to the project")
        ("synth", boost::program_options::value<unsigned int>(), "The index of the synth which wavetables need to be changed");
    
    boost::program_options::options_description cmdOptions{"Options"};
    cmdOptions.add_options()
        ("help,h", "Help screen");
    
    boost::program_options::options_description options;
    options.add(cmdOptions).add(hidden);
    
    boost::program_options::positional_options_description positionalOptions;
    positionalOptions.add("project", 1);
    positionalOptions.add("wavetable", 1);
    positionalOptions.add("synth", 1);
    
    try
    {
        boost::program_options::variables_map vm;
        boost::program_options::command_line_parser parser(argc, argv);
        parser = parser.options(options);
        parser = parser.positional(positionalOptions);
        boost::program_options::store(parser.run(), vm);
        boost::program_options::notify(vm);
        
        if (vm.count("help"))
        {
            printHelp(cmdOptions);
            return 0;
        } else if (vm.count("project") && vm.count("wavetable") && vm.count("synth")) {
            return apply(vm["project"].as<std::string>(), vm["wavetable"].as<std::string>(), vm["synth"].as<unsigned int>());
        } else {
            printHelp(cmdOptions);
            return 0;
        }
    } catch (const boost::program_options::error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
    }

	return 0;
}
