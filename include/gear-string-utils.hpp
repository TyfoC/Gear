#pragma once

#include "gear-scanner.hpp"

namespace Gear {
	struct StringPosition {
		size_t	Line;
		size_t	Column;
	};

	StringPosition GetStringPosition(const std::string &source, size_t position);
	size_t GetCharactersCountBeforeLineBreak(const std::string &source, size_t position);
}