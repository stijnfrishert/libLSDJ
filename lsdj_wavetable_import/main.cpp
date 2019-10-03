/*
 
 This file is a part of liblsdj, a C library for managing everything
 that has to do with LSDJ, software for writing music (chiptune) with
 your gameboy. For more information, see:
 
 * https://github.com/stijnfrishert/liblsdj
 * http://www.littlesounddj.com
 
 --------------------------------------------------------------------------------
 
 MIT License
 
 Copyright (c) 2018 - 2019 Stijn Frishert
 
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
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "../common/common.hpp"
#include "../liblsdj/project.h"
#include "../liblsdj/sav.h"
#include "wavetable_importer.hpp"

void printHelp(const boost::program_options::options_description& desc)
{
    std::cout << "lsdj-wavetable-import source.lsdsng wavetables.snt --[synth 0-F | index 00-FF]\n\n"
              << "Version: " << lsdj::VERSION << "\n\n"
              << desc << "\n";

    std::cout << "LibLsdj is open source and freely available to anyone.\nIf you'd like to show your appreciation, please consider\n  - buying one of my albums (https://4ntler.bandcamp.com)\n  - donating money through PayPal (https://paypal.me/4ntler).\n";
}

unsigned char parseSynthIndex(const std::string& str)
{
    return static_cast<unsigned char>(std::stoul(str, nullptr, 16)) * 16;
}

unsigned char parseIndex(const std::string& str)
{
    return static_cast<unsigned char>(std::stoul(str, nullptr, 16));
}

int main(int argc, char* argv[])
{
    boost::program_options::options_description hidden{"Hidden"};
    hidden.add_options()
        ("input", boost::program_options::value<std::vector<std::string>>(), "A .lsdsng project, .sav or wavetable (.snt)");
    
    boost::program_options::options_description cmd{"Options"};
    cmd.add_options()
        ("help,h", "Help screen")
        ("index,i", boost::program_options::value<std::string>(), "The wavetable index 00-FF where the wavetable data should be written")
        ("synth,s", boost::program_options::value<std::string>(), "The synth number 0-F where the wavetable data should be written")
        ("zero,0", "Pad the synth with empty wavetables if the .snt file < 256 bytes")
        ("force,f", "Force writing the wavetables, even though non-default data may be in them")
        ("output,o", boost::program_options::value<std::string>(), "The output .lsdsng to write to")
        ("verbose,v", "Verbose output");
    
    boost::program_options::options_description options;
        options.add(cmd).add(hidden);
    
    boost::program_options::positional_options_description positionalOptions;
    positionalOptions.add("input", 2);
    
    try
    {
        boost::program_options::variables_map vm;
        boost::program_options::command_line_parser parser(argc, argv);
        parser.options(options).positional(positionalOptions);
        boost::program_options::store(parser.run(), vm);
        boost::program_options::notify(vm);
        
        if (vm.count("help"))
        {
            printHelp(cmd);
            return 0;
        }
        else if (vm.count("input") == 1 && (vm.count("synth") || vm.count("index")))
        {
            lsdj::WavetableImporter importer;
            
            const auto inputs = vm["input"].as<std::vector<std::string>>();
            
            std::string source;
            std::string wavetable;
            
            lsdj_error_t* error = nullptr;
            if (lsdj_sav_is_likely_valid_file(inputs[0].c_str(), &error) ||
                lsdj_project_is_likely_valid_lsdsng_file(inputs[0].c_str(), &error))
            {
                source = inputs[0];
                wavetable = inputs[1];
            }
            else if (lsdj_sav_is_likely_valid_file(inputs[1].c_str(), &error) ||
                     lsdj_project_is_likely_valid_lsdsng_file(inputs[1].c_str(), &error))
            {
                source = inputs[1];
                wavetable = inputs[0];
            }
            else
            {
                std::cerr << "Neither of the inputs is likely a valid .lsdsng or .sav" << std::endl;
                return 1;
            }
            
            importer.outputName = vm.count("output") ? vm["output"].as<std::string>() : source;
            importer.wavetableIndex = vm.count("synth") ? parseSynthIndex(vm["synth"].as<std::string>()) : parseIndex(vm["index"].as<std::string>());
            importer.zero = vm.count("zero");
            importer.force = vm.count("force");
            importer.verbose = vm.count("verbose");
            
            return importer.import(source, wavetable) ? 0 : 1;
        } else {
            printHelp(cmd);
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
