#include <gear-preprocessor.h>

/**
 * @brief Prerocesses input string
 * @param source string to be preprocessed
 * @return gear_preprocessing_result_t
 */
gear_preprocessing_result_t gear_preprocess(const char* source) {
	gear_preprocessing_result_t result;
	result.result = (char*)calloc(1, sizeof(char));
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
				if ((i += subLength) >= length) break;
				if (!gear_append_letters(&result.result, "@", 1)) {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}
			}
			else if (!memcmp(&source[i], "join", subLength)) {
				if ((i += subLength) >= length) break;
				GEAR_PREPROCESSOR_SKIP_TO_USEFUL(result.result, source, i, subLength, length);
				if (i >= length) break;
			}
			else {
				result.status = GEAR_STATUS_UNKNOWN_PREPROCESSOR_DIRECTIVE;
				return result;
			}
		}
		else {
			size_t subLength = gear_get_identifier_length(&source[i]);
			if (subLength) {
				if (!gear_resize_string(&result.result, strlen(result.result) + subLength)) {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}

				memcpy(&result.result[strlen(result.result)], &source[i], subLength);
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

	result.status = GEAR_STATUS_SUCCESS;
	return result;
}