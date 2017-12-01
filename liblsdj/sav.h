#ifndef LSDJ_SAV_H
#define LSDJ_SAV_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#include "error.h"
#include "project.h"
#include "song.h"
#include "vio.h"
    
typedef struct lsdj_sav_t lsdj_sav_t;

// Create/free saves
lsdj_sav_t* lsdj_new_sav(lsdj_error_t** error);
void lsdj_free_sav(lsdj_sav_t* sav);
    
// Deserialize a sav
lsdj_sav_t* lsdj_read_sav(lsdj_vio_t* vio, lsdj_error_t** error);
lsdj_sav_t* lsdj_read_sav_from_file(const char* path, lsdj_error_t** error);
lsdj_sav_t* lsdj_read_sav_from_memory(const unsigned char* data, size_t size, lsdj_error_t** error);
    
// Serialize a sav
void lsdj_write_sav(const lsdj_sav_t* sav, lsdj_vio_t* vio, lsdj_error_t** error);
void lsdj_write_sav_to_file(const lsdj_sav_t* sav, const char* path, lsdj_error_t** error);
void lsdj_write_sav_to_memory(const lsdj_sav_t* sav, unsigned char* data, size_t size, lsdj_error_t** error);
    
// Change data in a sav
void lsdj_sav_set_song(lsdj_sav_t* sav, lsdj_song_t* song);
lsdj_song_t* lsdj_sav_get_song(const lsdj_sav_t* sav);
void lsdj_sav_copy_active_project(lsdj_sav_t* sav, unsigned char index, lsdj_error_t** error);
unsigned char lsdj_sav_get_active_project(const lsdj_sav_t* sav);
unsigned int lsdj_sav_get_project_count(const lsdj_sav_t* sav);
void lsdj_sav_set_project(lsdj_sav_t* sav, unsigned char index, lsdj_project_t* project, lsdj_error_t** error);
lsdj_project_t* lsdj_sav_get_project(const lsdj_sav_t* sav, unsigned char project);

#ifdef __cplusplus
}
#endif

#endif
