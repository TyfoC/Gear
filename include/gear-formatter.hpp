#pragma once

#include "gear-token.hpp"
#include "gear-json.hpp"

namespace Gear {
	std::string FormatCharacter(char character);
	std::string FormatString(const std::string source);
	JsonElement TokenToJsonElement(const Token &token);
}