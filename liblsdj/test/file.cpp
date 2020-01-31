#include "file.hpp"

#include <fstream>

std::vector<std::uint8_t> readFileContents(std::string_view path)
{
	std::ifstream stream(RESOURCES_FOLDER "raw/happy_birthday.raw", std::ios::binary);
	stream.unsetf(std::ios::skipws);

    // Get the size
    stream.seekg(0, std::ios::end);
    std::vector<std::uint8_t> bytes;
    bytes.reserve(static_cast<unsigned long>(stream.tellg()));
    stream.seekg(0, std::ios::beg);

    bytes.insert(bytes.begin(),
    			 std::istream_iterator<std::uint8_t>(stream),
				 std::istream_iterator<std::uint8_t>());

    return bytes;
}