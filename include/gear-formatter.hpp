#pragma once

#include "gear-json.hpp"
#include "gear-lexer.hpp"

namespace Gear {
	std::string FormatCharacter(char character);
	std::string FormatString(const std::string source);
	JsonElement_t TokenToJsonElement(const Token_t token);
	JsonElement_t LexemeToJsonElement(const std::string source, const Lexeme_t lexeme);
}