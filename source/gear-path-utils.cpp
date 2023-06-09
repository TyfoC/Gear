#include <gear-path-utils.hpp>

std::string Gear::GetDirectoryPathFromFilePath(const std::string &filePath) {
	size_t length = filePath.length();
	size_t position = length - 1;
	for (; position < length; position--) if (filePath[position] == '/' || filePath[position] == '\\') break;
	return position >= length ? "" : filePath.substr(0, position);
}