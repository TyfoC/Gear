#include <gear-file-utils.hpp>

static struct stat GearBuffFileStats;

size_t Gear::GetFileLength(std::ifstream &handle) {
	std::streampos currentPos = handle.tellg();
	handle.seekg(0, std::ios::end);
	size_t result = (size_t)handle.tellg();
	handle.seekg(currentPos, std::ios::beg);
	return result;
}

std::string Gear::ReadTextFile(std::ifstream &handle) {
	std::string result;
	result.reserve(GetFileLength(handle));
	result.assign((std::istreambuf_iterator<char>(handle)), std::istreambuf_iterator<char>());
	return result;
}

int32_t Gear::GetFileUniqueID(const std::string &filePath) {
	stat(filePath.c_str(), &GearBuffFileStats);
	return (int32_t)((GearBuffFileStats.st_uid << 16) | GearBuffFileStats.st_gid);
}