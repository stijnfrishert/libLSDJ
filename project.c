#include <stdio.h>

#include "project.h"

void lsdj_write_lsdsng(lsdj_project_t* project, const char* path, lsdj_error_t** error)
{
    FILE* file = fopen(path, "w");
    if (file == NULL)
    {
        return lsdj_create_error(error, "could not open file for writing");
    }
    
    fwrite(project->name, 8, 1, file);
    fwrite(&project->version, 1, 1, file);
    fwrite(project->compressed_data.data, project->compressed_data.size, 1, file);
    
    fclose(file);
}
