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
#include "wavetable_importer.hpp"

void printHelp(const boost::program_options::options_description& desc)
{
    std::cout << "lsdj-wavetable-import source.lsdsng -w wavetables.snt [-s 0-F | -i 00-FF]\n\n" << desc;
}

unsigned char parseSynthIndex(const std::string& str)
{
    assert(!str.empty());
    if (str.size() == 1)
        return static_cast<unsigned char>(std::stoul(str, nullptr, 16));
    else
        return static_cast<unsigned char>(std::stoul(str));
}

unsigned char parseIndex(const std::string& str)
{
    return static_cast<unsigned char>(std::stoul(str, nullptr, 16));
}

int main(int argc, char* argv[])
{
    boost::program_options::options_description hidden{"Hidden"};
    hidden.add_options()
        ("input", "The .lsdsng project or .sav to which the wavetable should be applied");
    
    boost::program_options::options_description cmd{"Options"};
    cmd.add_options()
        ("help,h", "Help screen")
        ("wavetable,w", boost::program_options::value<std::string>()->required(), "The .snt wavetable file to import")
        ("index,i", "The wavetable index 00-FF where the wavetable data should be written")
        ("synth,s", "The synth number 0-F where the wavetable data should be written")
        ("zero,0", "Pad the synth with empty wavetables if the .snt file < 256 bytes")
        ("force,f", "Force writing the wavetables, even though non-default data may be in them")
        ("output,o", boost::program_options::value<std::string>(), "The output .lsdsng to write to")
        ("verbose,v", "Verbose output");
    
    boost::program_options::options_description options;
    options.add(cmd).add(hidden);
    
    boost::program_options::positional_options_description positionalOptions;
    positionalOptions.add("input", 1);
    
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
            printHelp(cmd);
            return 0;
        }
        else if (vm.count("input") && vm.count("wavetable") && (vm.count("synth") || vm.count("index")))
        {
            lsdj::WavetableImporter importer;
            
            const auto destination = vm["input"].as<std::string>();
            
            importer.outputName = vm.count("output") ? vm["output"].as<std::string>() : destination;
            importer.wavetableIndex = vm.count("synth") ? parseSynthIndex(vm["synth"].as<std::string>()) : parseIndex(vm["index"].as<std::string>());
            importer.zero = vm.count("zero");
            importer.force = vm.count("force");
            importer.verbose = vm.count("verbose");
            
            return importer.import(destination, vm["wavetable"].as<std::string>()) ? 0 : 1;
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
