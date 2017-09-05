#include <cassert>
#include <iostream>

#include <lsdj/sav.h>

using namespace std;

int main(int argc, char* argv[])
{
    lsdj_error_t* error = nullptr;
    lsdj_sav_t* sav = lsdj_open("/Users/dsperados/Desktop/lsdj.sav", &error);
    if (sav == nullptr)
    {
        std::runtime_error exception(lsdj_get_error_c_str(error));
        lsdj_free_error(error);
        throw exception;
    }
    
    for (auto i = 0; i < sav->project_count; ++i)
    {
        auto& project = sav->projects[i];
        cout << string(project.name, 8) << " (" << std::to_string((int)project.version) << ")" << endl;
    }
    
    lsdj_close(sav);
    
    return 0;
}
