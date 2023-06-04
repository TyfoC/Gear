#include <gear-file.h>

gfile_t gopen_file(const char* path, uint8_t mode) {
	gfile_t result;
	char modeString[3] = { mode & GEAR_FILE_MODE_WRITE ? 'w' : 'r', mode & GEAR_FILE_MODE_BINARY ? 'b' : 0, 0 };
	if (!(result.handle = fopen(path, modeString))) return result;

	size_t pathLen = strlen(path);
	result.path = calloc(pathLen + 1, sizeof(char));
	memcpy(result.path, path, pathLen + 1);
	
	result.mode = mode;
	return result;
}

void gclose_file(gfile_t value) {
	if (value.path) free(value.path);
	if (value.handle) {
		fclose(value.handle);
		value.handle = 0;
	}
}

size_t gget_file_size(const gfile_t value) {
	if (!GEAR_FILE_IS_OPEN(value)) return NPOS;

	long pos = ftell(value.handle);
	fseek(value.handle, 0, SEEK_END);
	size_t result = (size_t)ftell(value.handle);
	fseek(value.handle, pos, SEEK_SET);
	return result;
}

char* gread_file(const gfile_t value) {
	if (!GEAR_FILE_IS_OPEN(value)) return NULL;

	size_t fileSize = gget_file_size(value);
	if (fileSize == NPOS) return NULL;

	char* result = (char*)calloc(fileSize + (value.mode & GEAR_FILE_MODE_BINARY ? 0 : 1), sizeof(char));
	if (!result) return NULL;

	size_t cntRead = fread(result, sizeof(char), fileSize, value.handle);
	HIDE(cntRead);

	if (value.mode & GEAR_FILE_MODE_BINARY) result[fileSize] = 0;
	return result;
}

gbool_t gwrite_file_string(const gfile_t value, const char* source) {
	if (!GEAR_FILE_IS_OPEN(value)) return FALSE;

	fputs(source, value.handle);
	return TRUE;
}