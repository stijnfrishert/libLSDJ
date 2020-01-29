#include <error.h>

#include <catch2/catch.hpp>

using namespace Catch;

TEST_CASE( "Error creation and querying", "[error]" )
{
	SECTION( "Retrieve description from an error" )
	{
		auto error = lsdj_error_new("Hello");

		REQUIRE( lsdj_error_get_description_length(error) == 5 );
		REQUIRE_THAT( lsdj_error_get_description(error), Equals("Hello") );

		lsdj_error_free(error);
	}

	SECTION( "Creating an optional error" )
	{
		REQUIRE_NOTHROW(lsdj_error_optional_new(NULL, "Hi there"));

		lsdj_error_t* error;
		lsdj_error_optional_new(&error, "Hallo");

		REQUIRE( lsdj_error_get_description_length(error) == 5 );
		REQUIRE_THAT( lsdj_error_get_description(error), Equals("Hallo") );
	}
}