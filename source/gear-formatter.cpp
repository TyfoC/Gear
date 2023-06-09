#include <gear-formatter.hpp>

std::string Gear::FormatCharacter(char character) {
	switch (character) {
		case '\t':
			return "\\t";
		case '\v':
			return "\\v";
		case '\f':
			return "\\f";
		case '\a':
			return "\\a";
		case '\b':
			return "\\b";
		case '\r':
			return "\\r";
		case '\n':
			return "\\n";
		case '\\':
			return "\\\\";
		case '\'':
			return "\\\'";
		case '\"':
			return "\\\"";
	}

	char cStr[2] = { character, 0 };
	return std::string(cStr);
}

std::string Gear::FormatString(const std::string source) {
	std::string result = "";
	for (const char &element : source) result += FormatCharacter(element);
	return result;
}

Gear::JsonElement Gear::TokenToJsonElement(const Token &token) {
	JsonElement element = { JsonElement::TYPE_OBJECT, "", {}, {} };
	element.Values.push_back({ JsonElement::TYPE_DEC, "group", std::to_string((size_t)token.Group), {} });
	element.Values.push_back({ JsonElement::TYPE_DEC, "type", std::to_string((size_t)token.Type), {} });
	element.Values.push_back({ JsonElement::TYPE_DEC, "position", std::to_string((size_t)token.Position), {} });
	element.Values.push_back({ JsonElement::TYPE_STRING, "lexeme", FormatString(token.Lexeme), {} });
	return element;
}