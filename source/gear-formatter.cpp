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

Gear::JsonElement_t Gear::TokenToJsonElement(const Token_t token) {
	JsonElement_t element = { JsonElementType::OBJECT, "", {}, {} };
	element.Values.push_back({ JsonElementType::STRING, "pattern", FormatString(token.Pattern), {} });
	element.Values.push_back({ JsonElementType::DEC, "group", std::to_string((size_t)token.Group), {} });
	element.Values.push_back({ JsonElementType::DEC, "type", std::to_string((size_t)token.Type), {} });
	return element;
}

Gear::JsonElement_t Gear::LexemeToJsonElement(const std::string source, const Lexeme_t lexeme) {
	JsonElement_t element = { JsonElementType::OBJECT, "", {}, {} };
	element.Values.push_back({ JsonElementType::STRING, "value", FormatString(source.substr(lexeme.Position, lexeme.Length)), {} });
	element.Values.push_back({ JsonElementType::DEC, "group", std::to_string((size_t)lexeme.Group), {} });
	element.Values.push_back({ JsonElementType::DEC, "type", std::to_string((size_t)lexeme.Type), {} });
	element.Values.push_back({ JsonElementType::DEC, "position", std::to_string(lexeme.Position), {} });
	element.Values.push_back({ JsonElementType::DEC, "length", std::to_string(lexeme.Length), {} });
	return element;
}