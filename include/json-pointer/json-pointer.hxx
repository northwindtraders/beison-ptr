#pragma once

#include <beison.h>
#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

namespace beison::ptr
{

struct JsonPointerMember
{
	explicit JsonPointerMember(std::string value) : field_(std::move(value)), index_(static_cast<size_t>(-1)) {}
	explicit JsonPointerMember(size_t value) : field_(), index_(value) {}

	const beison::Value* get(const beison::Value& v) const
	{
		if (!field_.empty())
		{
			// should be an object
			if (v.is_object())
			{
				return v.object().get(field_);
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			// should be an array
			if (v.is_array() && v.array().size() > index_)
			{
				return &v.array()[index_];
			}
			else
			{
				return nullptr;
			}
		}
	}

	beison::Value* get(beison::Value& v) const
	{
		if (index_ == static_cast<size_t>(-1))
		{
			// should be an object
			if (v.is_object())
			{
				return v.object().get(field_);
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			// should be an array
			if (v.is_array() && v.array().size() > index_)
			{
				return &v.array()[index_];
			}
			else
			{
				return nullptr;
			}
		}
	}
private:
	std::string field_;
	size_t index_;
};

inline std::vector<JsonPointerMember> parse_json_pointer(const char* string)
{
	std::vector<JsonPointerMember> result;
	std::string value_storage;
	for (auto it = string;;) {
		if (*it == '\0')
		{
			return result;
		}

		if (*it == '[')
		{
			const char* value_begin = it + 1;
			const char* value_end = value_begin;
			while (*value_end != '\0' && *value_end != ']')
			{
				++value_end;
			}

			if (*value_end == '\0')
			{
				fmt::print(stderr, "invalid json pointer; expected a `]`, got eof\n");
				std::abort();
			}
			value_storage.assign(value_begin, value_end);
			if (value_storage.empty())
			{
				fmt::print(stderr, "invalid json pointer; expected digits before `]`\n");
				std::abort();
			}

			size_t value = 0;
			if (sscanf(value_storage.c_str(), "%zu", &value) != 1)
			{
				fmt::print(stderr, "invalid integer in index: {}\n", value_storage);
				std::abort();
			}

			result.emplace_back(value);
			it = value_end + 1;
			continue;
		}

		if (*it == '.')
		{
			const char* value_begin = it + 1;
			const char* value_end = value_begin;
			while (*value_end != '\0' && *value_end != '[' && *value_end != '.')
			{
				++value_end;
			}

			value_storage.assign(value_begin, value_end);
			result.emplace_back(std::move(value_storage));
			it = value_end;
			continue;
		}

		fmt::print(stderr, "invalid json pointer; expected either `.`, `[`, or eof to start, got `{}`\n", *it);
		std::abort();
	}
}

inline Value* index_json(Value& v, const std::vector<JsonPointerMember>& ptr)
{
	beison::Value* current = &v;
	for (const auto& el : ptr)
	{
		current = el.get(*current);
		if (!current)
		{
			return nullptr;
		}
	}
	return current;
}

}
