#include <stdio.h>

#include "project.h"

void lsdj_write_lsdsng(lsdj_project_t* project, const char* path, lsdj_error_t** error)
{
    FILE* file = fopen(path, "w");
    if (file == NULL)
    {
        return lsdj_create_error(error, "could not open file for writing");
    }
    
    fwrite(project->name, sizeof(project->name), 1, file);
    fwrite(&project->version, sizeof(project->version), 1, file);
    fwrite(&project->compressed_data.data, sizeof(project->compressed_data.block_count * BLOCK_SIZE), 1, file);
    
    fclose(file);
}
