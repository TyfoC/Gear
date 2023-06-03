#include <gear-preprocessor.h>

/**
 * @brief Prerocesses input string
 * @param source string to be preprocessed
 * @return gear_preprocessing_result_t
 */
gear_preprocessing_result_t gear_preprocess(const char* source, const char* srcFileDirectoryPath) {
	gear_preprocessing_result_t result = { GEAR_STATUS_SUCCESS, (char*)calloc(1, sizeof(char)) };
	result.result[0] = 0;

	size_t length = strlen(source), i = 0, redundantLength;
	while (i < length) {
		GEAR_PREPROCESSOR_JUMP_TO_USEFUL(result.result, source, i, redundantLength, length, result, result.status);
		if (i >= length) break;

		if (source[i] == GEAR_PREPROCESSOR_SYMBOL) {
			if ((i += 1) >= length) {
				result.status = GEAR_STATUS_MISSING_PREPROCESSOR_DIRECTIVE;
				return result;
			}

			GEAR_PREPROCESSOR_JUMP_TO_USEFUL(result.result, source, i, redundantLength, length, result, result.status);
			if (i >= length) {
				result.status = GEAR_STATUS_MISSING_PREPROCESSOR_DIRECTIVE;
				return result;
			}

			size_t subLength = gear_get_identifier_length(&source[i]);
			if (!subLength) {
				result.status = GEAR_STATUS_MISSING_PREPROCESSOR_DIRECTIVE;
				return result;
			}

			if (!memcmp(&source[i], "dog", subLength)) {
				if (!gear_append_letters(&result.result, "@", 1)) {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}
				
				i += subLength;

				if (source[i] == '@') {
					if ((i += 1) >= length) break;
					GEAR_PREPROCESSOR_SKIP_TO_USEFUL(result.result, source, i, subLength, length);
				}
			}
			else if (!memcmp(&source[i], "join", subLength)) {
				if ((i += subLength) >= length) break;
				GEAR_PREPROCESSOR_SKIP_TO_USEFUL(result.result, source, i, subLength, length);
				if (i >= length) break;
			}
			else if (!memcmp(&source[i], "preinc", subLength) || !memcmp(&source[i], "rawinc", subLength)) {
				unsigned char preprocessFile = source[i] == 'p';
				if ((i += subLength) >= length) {
					result.status = GEAR_STATUS_MISSING_FILE_PATH;
					return result;
				}

				GEAR_PREPROCESSOR_SKIP_TO_USEFUL(result.result, source, i, subLength, length);
				if (i >= length || (source[i] != '<' && source[i] != '\"')) {
					result.status = GEAR_STATUS_MISSING_FILE_PATH;
					return result;
				}
				
				unsigned char fileFromCurrentFolder = source[i] == '\"';
				size_t nextPosition = gear_find_char(source, fileFromCurrentFolder ? '\"' : '>', i + 1);
				if (nextPosition == GEAR_NPOS) {
					result.status = GEAR_STATUS_MISSING_FILE_PATH;
					return result;
				}

				char* fileName = gear_get_substring(source, i + 1, nextPosition - i - 1);
				if (!fileName) {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}

				char* filePath = gear_copy_string(srcFileDirectoryPath);
				if (!filePath || !gear_append_string(&filePath, fileName)) {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}

				unsigned char fileMode = GEAR_FILE_MODE_READ | GEAR_FILE_MODE_TEXT;
				gear_file_t file = gear_open_file(filePath, fileMode);
				if (!GEAR_FILE_IS_OPEN(file)) {
					result.status = GEAR_STATUS_FAILED_TO_OPEN_FILE;
					return result;
				}

				char* fileData = gear_read_file(file);
				if (!fileData) {
					result.status = GEAR_STATUS_FAILED_TO_READ_FILE;
					return result;
				}

				//	include content without preprocessing it 
				if (!preprocessFile && !gear_append_string(&result.result, fileData)) {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}
				else if (preprocessFile) {	//	preprocess content and include it
					gear_preprocessing_result_t subResult = gear_preprocess(fileData, srcFileDirectoryPath);
					if (subResult.status) return subResult;

					if (!gear_append_string(&result.result, subResult.result)) {
						result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
						return result;
					}
				}

				free(fileData);
				gear_close_file(file);
				free(filePath);
				free(fileName);
				i = nextPosition + 1;
			}
			else {
				result.status = GEAR_STATUS_UNKNOWN_PREPROCESSOR_DIRECTIVE;
				return result;
			}
		}
		else {
			size_t subLength = gear_get_identifier_length(&source[i]);
			if (subLength) {
				if (!gear_append_letters(&result.result, &source[i], subLength))  {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}
				
				i += subLength;
			}
			else if ((subLength = gear_get_string_literal_length(&source[i], '\'')) || (subLength = gear_get_string_literal_length(&source[i], '\"'))) {
				if (!gear_append_letters(&result.result, &source[i], subLength)) {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}

				i += subLength;
			}
			else {
				size_t resultLength = strlen(result.result);
				char* tmpStr = (char*)realloc(result.result, (resultLength + 2) * sizeof(char));
				if (!tmpStr) {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}

				result.result = tmpStr;
				result.result[resultLength] = source[i];
				result.result[resultLength + 1] = 0;
				i += 1;
			}
		}
	}
	
	return result;
}