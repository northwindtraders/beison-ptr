#include <beison.h>
#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

#include <json-pointer/json-pointer.hxx>

static std::string read_file(const std::filesystem::path& p, std::error_code& ec)
{
	ec.clear();
	std::fstream file_stream(p, std::ios_base::in, std::ios_base::binary);

	if (file_stream.fail())
	{
		ec = std::make_error_code(std::errc::no_such_file_or_directory);
		return {};
	}

	file_stream.seekg(0, file_stream.end);
	auto length = file_stream.tellg();
	file_stream.seekg(0, file_stream.beg);

	if (length == std::streampos(-1))
	{
		ec = std::make_error_code(std::errc::io_error);
		return {};
	}

	std::string output;
	output.resize(static_cast<size_t>(length));
	file_stream.read(output.data(), length);
	return output;
}


int main(int argc, char** argv) {
	if (argc != 3)
	{
		fmt::print(stderr, "{} must be passed 2 arguments (was passed {})\n", argv[0], argc - 1);
		return 1;
	}

	std::error_code ec;
	auto json_text = read_file(argv[1], ec);
	if (ec)
	{
		fmt::print(stderr, "Failed to read file {}: {}\n", argv[1], ec.message());
		return 1;
	}
	fmt::print(stderr, "past the read file\n");

	auto obj = beison::parse(json_text, ec);
	if (ec)
	{
		fmt::print(stderr, "Failed to parse file {}: {}\n", argv[1], ec.message());
		return 1;
	}
	fmt::print(stderr, "past the parse file\n");

	auto json_ptr = beison::ptr::parse_json_pointer(argv[2]);
	fmt::print(stderr, "past the json pointer\n");
	auto current = beison::ptr::index_json(obj, json_ptr);
	if (!current)
	{
		fmt::print(stderr, "Failed to find json pointer in text\n");
		return 1;
	}
	fmt::print(stderr, "past the index\n");

	fmt::print("{}", beison::stringify(*current));
}
