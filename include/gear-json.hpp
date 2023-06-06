#pragma once

#include <string>
#include <vector>

namespace Gear {
	enum class JsonElementType {
		UNDEFINED,
		STRING,
		HEX,
		DEC,
		OCT,
		BIN,
		BOOLEAN,
		ARRAY,
		OBJECT
	};

	struct JsonElement_t {
		JsonElementType				Type;
		std::string					Name;	//	if `Type` != JsonElementType::OBJECT
		std::string					Value;	//	if `Type` != JsonElementType::ARRAY && `Type` != JsonElementType::OBJECT
		std::vector<JsonElement_t>	Values;	//	if `Type` == JsonElementType::ARRAY || `Type` == JsonElementType::OBJECT
	};

	std::string StringifyJsonElement(const JsonElement_t jsonElement, bool expandNestedElements = true, size_t nestingLevel = 0);
}