#include <gear-file.h>

char* gear_read_file_text(const char* filePath) {
	char* result;
	long fsize;
	FILE* inputFile = fopen(filePath, "r");
	if (!inputFile) return 0;

	fseek(inputFile, 0, SEEK_END);
	fsize = ftell(inputFile);
	fseek(inputFile, 0, SEEK_SET);

	result = (char*)calloc(fsize + 1, sizeof(char));

	size_t bytesCount = fread(result, fsize, 1, inputFile);
	GEAR_UNREFERENCED(bytesCount);

	fclose(inputFile);
	result[fsize] = 0;

	return result;
}