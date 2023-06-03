#include <gear-path.h>

/**
 * @brief /path/to/file.txt -> /path/to/
 * 
 * @param filePath 
 * @return char* 
 */
char* gear_get_file_path_directory(const char* filePath) {
	size_t length = strlen(filePath);
	size_t limiterPos = length;
	do {
		if (filePath[limiterPos] == GEAR_LINUX_PATH_LIMITER || filePath[limiterPos] == GEAR_WINDOWS_PATH_LIMITER) break;
		limiterPos -= 1;
	} while (limiterPos < length);

	if (limiterPos >= length) limiterPos = 1;

	char* result = (char*)calloc(limiterPos + 2, sizeof(char));
	if (!result) return NULL;

	memcpy(result, filePath, limiterPos + 1);
	result[limiterPos + 1] = 0;
	return result;
}