#include <gear-file.h>
#include <gear-preprocessor.h>

void print_info(void) {
	printf("Gear programming language toolkit by github.com/TyfoC\nVersion: %s\n", GearVersionString);
}

void print_help(void) {
	printf("gear32/64 <action> [input_file_path [flags]]\nActions:\n");
	printf("\tinfo - show version info\n");
	printf("\thelp - show help\n");
	printf("\tpreprocess - preprocess input files\n");
	printf("Flags:\n");
	printf("\t-I <include dir path> - include directory\n");
	printf("\t-o - select output file path\n");
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Error: no action (type `help` to list actions)!\n");
		return GEAR_STATUS_NO_ACTION;
	}

	if (!strcmp(argv[1], "info")) print_info();
	else if (!strcmp(argv[1], "help")) print_help();
	else if (!strcmp(argv[1], "preprocess")) {
		if (argc < 3) {
			printf("Error: missing file path!\n");
			return GEAR_STATUS_MISSING_FILE_PATH;
		}

		gear_file_t file = gear_open_file(argv[2], GEAR_FILE_MODE_READ | GEAR_FILE_MODE_TEXT);
		if (!GEAR_FILE_IS_OPEN(file)) {
			printf("Error: failed to open `%s`!\n", argv[2]);
			return GEAR_STATUS_FAILED_TO_OPEN_FILE;
		}

		char* fileData = gear_read_file(file);
		if (!fileData) {
			printf("Error: failed to read `%s`!\nPossible causes: file does not exist/file cannot be read/not enough memory\n", argv[2]);
			return GEAR_STATUS_FAILED_TO_READ_FILE;
		}

		char* fileDirectoryPath = gear_get_file_path_directory(argv[2]);
		if (!fileDirectoryPath) {
			printf("Error: cannot get directory path from file path (not enough memory)!\n");
			return GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
		}

		gear_preprocessing_result_t result = gear_preprocess(fileData, fileDirectoryPath);
		if (result.status) {
			printf("Error: failed to preprocess `%s`!\n", fileDirectoryPath);
			return result.status;
		}

		printf("%s\n", result.result);

		free(fileDirectoryPath);
		free(fileData);
		gear_close_file(file);
	}
	else {
		printf("Error: indefinite action `%s`!\n", argv[1]);
		return GEAR_STATUS_INDEFINITE_ACTION;
	}

	return GEAR_STATUS_SUCCESS;
}