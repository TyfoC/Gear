#pragma once

#include <string>
#include <vector>

namespace Gear {
	struct JsonElement {
		enum {
			TYPE_UNDEFINED,
			TYPE_STRING,
			TYPE_CHAR,
			TYPE_HEX,
			TYPE_DEC,
			TYPE_OCT,
			TYPE_BIN,
			TYPE_BOOLEAN,
			TYPE_ARRAY,
			TYPE_OBJECT
		};

		size_t						Type;
		std::string					Name;	//	if `Type` != JsonElementType::OBJECT
		std::string					Value;	//	if `Type` != JsonElementType::ARRAY && `Type` != JsonElementType::OBJECT
		std::vector<JsonElement>	Values;	//	if `Type` == JsonElementType::ARRAY || `Type` == JsonElementType::OBJECT
	};

	std::string StringifyJsonElement(const JsonElement jsonElement, bool expandNestedElements = true, size_t nestingLevel = 0);
}