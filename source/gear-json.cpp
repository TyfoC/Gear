#include <gear-json.hpp>

std::string Gear::StringifyJsonElement(const JsonElement jsonElement, bool expandNestedElements, size_t nestingLevel) {
	std::string result = "";
	for (size_t i = 0; i < nestingLevel; i++) result += '\t';

	if (jsonElement.Name[0]) result += '\"' + jsonElement.Name + "\": ";

	if (jsonElement.Type == JsonElement::TYPE_ARRAY || jsonElement.Type == JsonElement::TYPE_OBJECT) {
		result += jsonElement.Type == JsonElement::TYPE_ARRAY ? "[" : "{";
		if (expandNestedElements) {
			result += '\n';
			for (const JsonElement &element : jsonElement.Values) {
				result += StringifyJsonElement(element, nestingLevel + 1);
				result += ",\n";
			}
			result.erase(result.length() - 2, 1);
		}
		else {
			result += ' ';
			for (const JsonElement &element : jsonElement.Values) {
				result += StringifyJsonElement(element, nestingLevel + 1);
				result += ", ";
			}
			result.erase(result.length() - 2, 1);
		}

		result += jsonElement.Type == JsonElement::TYPE_ARRAY ? "]" : "}";
	}
	else if (jsonElement.Type == JsonElement::TYPE_STRING) result += '\"' + jsonElement.Value + '\"';
	else if (jsonElement.Type == JsonElement::TYPE_CHAR) result += '\'' + jsonElement.Value + '\'';
	else if (jsonElement.Type == JsonElement::TYPE_HEX) result += "0x" + jsonElement.Value;
	else if (jsonElement.Type == JsonElement::TYPE_DEC) result += jsonElement.Value;
	else if (jsonElement.Type == JsonElement::TYPE_OCT) result += "0o" + jsonElement.Value;
	else if (jsonElement.Type == JsonElement::TYPE_BIN) result += "0b" + jsonElement.Value;
	else result += jsonElement.Value;

	return result;
}