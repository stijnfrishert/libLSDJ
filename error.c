#include <stdlib.h>

#include "error.h"

struct lsdj_error_t
{
    const char* message;
};

void lsdj_create_error(lsdj_error_t** error, const char* message)
{
    if (error == NULL)
        return;
    
    *error = malloc(sizeof(lsdj_error_t));
    (*error)->message = message;
}

void lsdj_free_error(lsdj_error_t* error)
{
    free(error);
}

const char* lsdj_get_error_c_str(lsdj_error_t* error)
{
    if (error == NULL)
        return NULL;
    else
        return error->message;
}
