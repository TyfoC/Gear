#include <gear-json.hpp>

std::string Gear::StringifyJsonElement(const JsonElement_t jsonElement, bool expandNestedElements, size_t nestingLevel) {
	std::string result = "";
	for (size_t i = 0; i < nestingLevel; i++) result += '\t';

	if (jsonElement.Name[0]) result += '\"' + jsonElement.Name + "\": ";

	if (jsonElement.Type == JsonElementType::ARRAY || jsonElement.Type == JsonElementType::OBJECT) {
		result += jsonElement.Type == JsonElementType::ARRAY ? "[" : "{";
		if (expandNestedElements) {
			result += '\n';
			for (const JsonElement_t &element : jsonElement.Values) {
				result += StringifyJsonElement(element, nestingLevel + 1);
				result += ",\n";
			}
			result.erase(result.length() - 2, 1);
		}
		else {
			result += ' ';
			for (const JsonElement_t &element : jsonElement.Values) {
				result += StringifyJsonElement(element, nestingLevel + 1);
				result += ", ";
			}
			result.erase(result.length() - 2, 1);
		}

		result += jsonElement.Type == JsonElementType::ARRAY ? "]" : "}";
	}
	else if (jsonElement.Type == JsonElementType::STRING) result += '\"' + jsonElement.Value + '\"';
	else if (jsonElement.Type == JsonElementType::HEX) result += "0x" + jsonElement.Value;
	else if (jsonElement.Type == JsonElementType::DEC) result += jsonElement.Value;
	else if (jsonElement.Type == JsonElementType::OCT) result += "0o" + jsonElement.Value;
	else if (jsonElement.Type == JsonElementType::BIN) result += "0b" + jsonElement.Value;
	else result += jsonElement.Value;

	return result;
}