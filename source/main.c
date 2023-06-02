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

		char* fileData = gear_read_file_text(argv[2]);
		if (!fileData) {
			printf("Error: failed to open `%s`!\n", argv[2]);
			return GEAR_STATUS_FAILED_TO_OPEN_FILE;
		}

		gear_preprocessing_result_t result = gear_preprocess(fileData);
		if (result.status) {
			printf("Error: failed to preprocess `%s`!\n", argv[2]);
			return result.status;
		}

		printf("%s\n", result.result);
		free(fileData);
	}
	else {
		printf("Error: indefinite action `%s`!\n", argv[1]);
		return GEAR_STATUS_INDEFINITE_ACTION;
	}

	return GEAR_STATUS_SUCCESS;
}