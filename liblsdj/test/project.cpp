#include <project.h>

#include <catch2/catch.hpp>

TEST_CASE( "Factorials are computed", "[factorial]" ) {
	lsdj_error_t* error = nullptr;
    lsdj_project_t* project = lsdj_project_new(&error);
    lsdj_project_free(project);
}