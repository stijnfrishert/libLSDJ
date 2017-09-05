#ifndef LSDJ_PROJECT_H
#define LSDJ_PROJECT_H

#ifdef __cplusplus
extern "C" {
#endif

//! Representation of a project within an LSDJ sav file
typedef struct
{
	char name[8];
    char version;
} lsdj_project_t;

#ifdef __cplusplus
}
#endif

#endif
