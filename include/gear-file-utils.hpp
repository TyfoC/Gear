#pragma once

#include <string>
#include <fstream>
#include <sys/stat.h>

namespace Gear {
	size_t GetFileLength(std::ifstream &handle);
	std::string ReadTextFile(std::ifstream &handle);
	int32_t GetFileUniqueID(const std::string &filePath);
}