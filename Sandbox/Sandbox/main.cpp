#include <cassert>
#include <iostream>
#include <sstream>

#include <lsdj/sav.h>

using namespace std;

int main(int argc, char* argv[])
{
    lsdj_error_t* error = nullptr;
    lsdj_sav_t* sav = lsdj_open_sav("/Users/dsperados/Desktop/lsdj/Chipwrecked-4ntler.sav", &error);
    if (error)
    {
        std::runtime_error exception(lsdj_get_error_c_str(error));
        lsdj_free_error(error);
        throw exception;
    }
    
    assert(sav != nullptr);
    
    for (auto i = 0; i < sav->project_count; ++i)
    {
        auto& project = sav->projects[i];
        
        auto length = strlen(project.name);
        
        stringstream stream;
        stream << "/Users/dsperados/Desktop/lsdj/";
        stream << string(project.name, length > 8 ? 8 : length);
        stream << "." << hex << (unsigned short)project.version;
        stream << ".lsdsng";
        
        lsdj_write_lsdsng(&project, stream.str().c_str(), &error);
        if (error)
        {
            std::runtime_error exception(lsdj_get_error_c_str(error));
            lsdj_free_error(error);
            throw exception;
        }
        
        cout << string(project.name, 8) << " (" << std::to_string((int)project.version) << ")" << endl;
        cout << (int)project.compressed_data.size << endl;
    }
    
    lsdj_close_sav(sav);
    
    return 0;
}
