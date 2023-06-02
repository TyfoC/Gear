#include <gear-file.h>

char* gear_read_file_text(const char* filePath) {
	char* result;
	FILE* inputFile = fopen(filePath, "r");
	fseek(inputFile, 0, SEEK_END);
	long fsize = ftell(inputFile);
	fseek(inputFile, 0, SEEK_SET);
	result = malloc(fsize + 1);
	fread(result, fsize, 1, inputFile);
	fclose(inputFile);
	result[fsize] = 0;

	return result;
}