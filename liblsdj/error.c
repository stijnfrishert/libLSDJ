#include <stdlib.h>
#include <string.h>

#include "error.h"

struct lsdj_error_t
{
    char* message;
};

void lsdj_create_error(lsdj_error_t** error, const char* message)
{
    if (error == NULL)
        return;
    
    *error = (lsdj_error_t*)malloc(sizeof(lsdj_error_t));
    size_t length = strlen(message) + 1; // Add one for the null-termination
    (*error)->message = malloc(length * sizeof(char));
    strncpy((*error)->message, message, length);
}

void lsdj_free_error(lsdj_error_t* error)
{
    if (error)
    {
        if (error->message)
        {
            free(error->message);
            error->message = NULL;
        }
        
        free(error);
    }
}

const char* lsdj_get_error_c_str(lsdj_error_t* error)
{
    if (error == NULL)
        return NULL;
    else
        return error->message;
}
