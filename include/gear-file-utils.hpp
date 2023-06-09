#pragma once

#include <string>
#include <fstream>

namespace Gear {
	size_t GetFileLength(std::ifstream &handle);
	std::string ReadTextFile(std::ifstream &handle);
}