#pragma once
#ifndef GEAR_UTILS
#define GEAR_UTILS

#include <string>

namespace Gear {
	size_t GetSymbolsCountBeforeLineBreak(const std::string& source, size_t position);
}

#endif