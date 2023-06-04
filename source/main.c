#include <main.h>

void print_info(void) {
	printf("Gear programming language toolkit by github.com/TyfoC\nVersion: %s\n", GEAR_VERSION_STRING);
}

void print_help(void) {
	printf("gear32/64 <action> [input_file_paths [flags]]\nActions:\n");
	printf("\tinfo - show version info\n");
	printf("\thelp - show help\n");
	printf("\tlex - generate lexeme table\n");
	printf("\t[N/A] preprocess - preprocess input files\n");
	printf("Flags:\n");
	printf("\t[N/A] -I <include dir path> - include directory\n");
	printf("\t-o - select output file path\n");
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");

	size_t action = ACTION_NOT_SELECTED;
	char** inputPaths = (char**)malloc(0);
	size_t inputPathsCount = 0;
	char* outputFilePath = 0;

	void* tmp;
	for (size_t i = 1; i < (size_t)argc; i++) {
		if (!strcmp(argv[i], "info")) action = ACTION_INFO;
		else if (!strcmp(argv[i], "help")) action = ACTION_HELP;
		else if (!strcmp(argv[i], "lex")) action = ACTION_LEX;
		else if (!strcmp(argv[i], "preprocess")) action = ACTION_PREPROCESS;
		else if (!strcmp(argv[i], "-o")) {
			if (i >= (size_t)argc - 1) break;
			if (outputFilePath) printf("Warning: output file path was already selected. It changed to `%s`!\n", argv[i + 1]);
			outputFilePath = argv[i + 1];
			i += 1;
		}
		else {
			tmp = (char**)gresize_array(inputPaths, sizeof(char*), inputPathsCount + 1);
			if (!tmp) {
				printf("Error: not enough memory!\n");
				return GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
			}

			inputPaths = tmp;
			inputPaths[inputPathsCount] = argv[i];
			inputPathsCount += 1;
		}
	}

	if (action == ACTION_NOT_SELECTED) {
		printf("Error: no action selected (type `help` to list actions)!\n");
		return GEAR_STATUS_NO_ACTION;
	}
	else if (action == ACTION_INFO) print_info();
	else if (action == ACTION_HELP) print_help();
	else if (action == ACTION_LEX) {
		if (inputPathsCount != 1) {
			printf("Warning: no input files//too many input files (must be one)!\n");
			return GEAR_STATUS_SUCCESS;
		}

		gfile_t file = gopen_file(inputPaths[0], GEAR_FILE_MODE_READ | GEAR_FILE_MODE_TEXT);
		if (!GEAR_FILE_IS_OPEN(file)) {
			printf("Error: failed to open `%s`!\n", inputPaths[0]);
			return GEAR_STATUS_FAILED_TO_OPEN_FILE;
		}

		char* fileData = gread_file(file);
		if (!fileData) {
			printf("Error: failed to read `%s`!\nPossible causes: file does not exist/file cannot be read/not enough memory\n", inputPaths[0]);
			return GEAR_STATUS_FAILED_TO_READ_FILE;
		}

		glexeme_t* result;
		size_t lexemesCount = glex(fileData, &result);
		if (lexemesCount == NPOS) {
			size_t unresolvedExprPos = lexemesCount ? result[lexemesCount - 1].position + result[lexemesCount - 1].length : 0;
			printf("Error: unresolved expression at %u!\n", (unsigned int)unresolvedExprPos);
			free(result);
			free(fileData);
			gclose_file(file);
			return GEAR_STATUS_UNRESOLVED_EXPRESSION;
		}

		char* formattedOutput = gformat_lexemes(fileData, result, lexemesCount);
		free(result);
		free(fileData);
		gclose_file(file);

		if (outputFilePath) {
			file = gopen_file(outputFilePath, GEAR_FILE_MODE_WRITE | GEAR_FILE_MODE_TEXT);
			if (!GEAR_FILE_IS_OPEN(file) || !gwrite_file_string(file, formattedOutput)) {
				printf("Error: failed to write file `%s`!\n", outputFilePath);
				free(formattedOutput);
				return GEAR_STATUS_FAILED_TO_OPEN_FILE;
			}

			gclose_file(file);
		}
		else printf("%s", formattedOutput);

		free(formattedOutput);
	}
	else if (action == ACTION_PREPROCESS) {
		if (inputPathsCount != 1) {
			printf("Warning: no input files//too many input files (must be one)!\n");
			return GEAR_STATUS_SUCCESS;
		}

		gfile_t file = gopen_file(inputPaths[0], GEAR_FILE_MODE_READ | GEAR_FILE_MODE_TEXT);
		if (!GEAR_FILE_IS_OPEN(file)) {
			printf("Error: failed to open `%s`!\n", inputPaths[0]);
			return GEAR_STATUS_FAILED_TO_OPEN_FILE;
		}

		char* fileData = gread_file(file);
		if (!fileData) {
			printf("Error: failed to read `%s`!\nPossible causes: file does not exist/file cannot be read/not enough memory\n", inputPaths[0]);
			return GEAR_STATUS_FAILED_TO_READ_FILE;
		}

		char* fileDirectoryPath = gget_file_path_directory(inputPaths[0]);
		if (!fileDirectoryPath) {
			printf("Error: not enough memory!\n");
			return GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
		}

		gpreprocessing_result_t result = gpreprocess(fileData, fileDirectoryPath);
		if (!result.completed_successfully) {
			free(fileDirectoryPath);
			free(fileData);
			gclose_file(file);
			printf("Error: failed to preprocess `%s`!\n", inputPaths[0]);
			return GEAR_STATUS_FAILED_TO_PREPROCESS;
		}

		free(fileDirectoryPath);
		free(fileData);
		gclose_file(file);

		if (outputFilePath) {
			file = gopen_file(outputFilePath, GEAR_FILE_MODE_WRITE | GEAR_FILE_MODE_TEXT);
			if (!GEAR_FILE_IS_OPEN(file) || !gwrite_file_string(file, result.output)) {
				printf("Error: failed to write file `%s`!\n", outputFilePath);
				free(result.output);
				return GEAR_STATUS_FAILED_TO_OPEN_FILE;
			}

			gclose_file(file);
		}
		else printf("%s", result.output);

		free(result.output);
	}

	/*
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
			printf("Error: failed to preprocess `%s`!\n", argv[2]);
			return result.status;
		}

		printf("%s\n", result.result);

		free(fileDirectoryPath);
		free(fileData);
		gear_close_file(file);
	}
	*/

	else {
		printf("Error: indefinite action `%s`!\n", argv[1]);
		return GEAR_STATUS_UNKNOWN_ACTION;
	}

	return GEAR_STATUS_SUCCESS;
}