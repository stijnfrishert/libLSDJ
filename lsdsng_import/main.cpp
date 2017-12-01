#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>

#include "../liblsdj/sav.h"

int handle_error(lsdj_error_t* error)
{
    std::cerr << "ERROR: " << lsdj_get_error_c_str(error) << std::endl;
    lsdj_free_error(error);
    return 1;
}

int importSongs(const std::vector<std::string>& songFiles, const std::string& outputFile, const char* savName)
{
    lsdj_error_t* error = nullptr;
    lsdj_sav_t* sav = savName ? lsdj_read_sav_from_file(boost::filesystem::absolute(savName).string().c_str(), &error) : lsdj_new_sav(&error);
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
    
    const auto active = lsdj_sav_get_active_project(sav);
    for (auto i = 0; i < songFiles.size(); ++i)
    {
        if (index == lsdj_sav_get_project_count(sav))
            break;
        
        lsdj_project_t* project = lsdj_read_lsdsng_from_file(boost::filesystem::absolute(songFiles[i]).string().c_str(), &error);
        if (error)
        {
            lsdj_free_sav(sav);
            return handle_error(error);
        }
        
        lsdj_sav_set_project(sav, index++, project, &error);
        if (error)
        {
            lsdj_free_project(project);
            lsdj_free_sav(sav);
            return handle_error(error);
        }
        
        if (i == 0 && active == 0xFF)
        {
            lsdj_sav_copy_active_project(sav, i, &error);
            if (error)
            {
                lsdj_free_project(project);
                lsdj_free_sav(sav);
                return handle_error(error);
            }
        }
    }
    
    lsdj_write_sav_to_file(sav, boost::filesystem::absolute(outputFile).string().c_str(), &error);
    if (error)
    {
        lsdj_free_sav(sav);
        return handle_error(error);
    }
    
    return 0;
}

int main(int argc, char* argv[])
{
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help screen")
        ("file,f", boost::program_options::value<std::vector<std::string>>(), ".lsdsng file(s), 0 or more")
        ("output,o", boost::program_options::value<std::string>()->default_value("out.sav"), "The output file (.sav)")
        ("sav,s", boost::program_options::value<std::string>(), "A sav file to append all .lsdsng's to");
    
    boost::program_options::positional_options_description positionalOptions;
    positionalOptions.add("file", -1);
    
    try
    {
        boost::program_options::variables_map vm;
        boost::program_options::command_line_parser parser(argc, argv);
        parser = parser.options(desc);
        parser = parser.positional(positionalOptions);
        boost::program_options::store(parser.run(), vm);
        boost::program_options::notify(vm);
        
        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        } else if (vm.count("file") && vm.count("output")) {
            return importSongs(vm["file"].as<std::vector<std::string>>(),
                               vm["output"].as<std::string>(),
                               vm.count("sav") ? vm["sav"].as<std::string>().c_str() : nullptr);
        } else {
            std::cout << desc << std::endl;
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
