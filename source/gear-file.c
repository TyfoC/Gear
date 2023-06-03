#include <gear-file.h>

gear_file_t gear_open_file(const char* path, unsigned char mode) {
	gear_file_t result;
	char modeString[3] = { mode & GEAR_FILE_MODE_WRITE ? 'w' : 'r', mode & GEAR_FILE_MODE_BINARY ? 'b' : 0, 0 };
	if (!(result.handle = fopen(path, modeString))) return result;

	result.path = gear_copy_string(path);
	result.mode = mode;
	return result;
}

void gear_close_file(gear_file_t value) {
	if (value.path) free(value.path);
	if (value.handle) {
		fclose(value.handle);
		value.handle = 0;
	}
}

size_t gear_get_file_size(const gear_file_t value) {
	if (!GEAR_FILE_IS_OPEN(value)) return GEAR_NPOS;

	long pos = ftell(value.handle);
	fseek(value.handle, 0, SEEK_END);
	size_t result = (size_t)ftell(value.handle);
	fseek(value.handle, pos, SEEK_SET);
	return result;
}

char* gear_read_file(const gear_file_t value) {
	if (!GEAR_FILE_IS_OPEN(value)) return NULL;

	size_t fileSize = gear_get_file_size(value);
	if (fileSize == GEAR_NPOS) return NULL;

	char* result = (char*)calloc(fileSize + (value.mode & GEAR_FILE_MODE_BINARY ? 0 : 1), sizeof(char));
	if (!result) return NULL;

	size_t cntRead = fread(result, sizeof(char), fileSize, value.handle);
	GEAR_UNREFERENCED(cntRead);

	if (value.mode & GEAR_FILE_MODE_BINARY) result[fileSize] = 0;
	return result;
}