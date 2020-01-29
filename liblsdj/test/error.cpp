#include <error.h>

#include <catch2/catch.hpp>

using namespace Catch;

SCENARIO( "Error creation and querying", "[error]" )
{
	GIVEN("An error is created" )
	{
		auto error = lsdj_error_new("Hello");

		REQUIRE( lsdj_error_get_description_length(error) == 5 );
		REQUIRE_THAT( lsdj_error_get_description(error), Equals("Hello") );

		lsdj_error_free(error);
	}
}