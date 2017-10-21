#ifndef LSDJ_ERROR_H
#define LSDJ_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

// Structure containing specific error details
typedef struct lsdj_error_t lsdj_error_t;
    
// Create an error with a given message
/*! Every call to lsdj_create_error() should be paired with one to lsdj_free_error() */
void lsdj_create_error(lsdj_error_t** error, const char* message);
    
// Free error data returned from an lsdj function call
/*! Every call to lsdj_create_error() should be paired with one to lsdj_free_error() */
void lsdj_free_error(lsdj_error_t* error);
    
// Retrieve a string description of an error
const char* lsdj_get_error_c_str(lsdj_error_t* error);
    
#ifdef __cplusplus
}
#endif

#endif
