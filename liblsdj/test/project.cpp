#include <project.h>

#include <catch2/catch.hpp>

using namespace Catch;

SCENARIO( "Project", "[project]" )
{
	GIVEN("An new project" )
	{
		auto project = lsdj_project_new();
	}
}