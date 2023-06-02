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

	size_t length = strlen(source), i = 0;
	while (i < length) {
		size_t redundantLength = gear_get_redundant_length(&source[i]);
		if (redundantLength) if (!gear_append_letters(&result.result, &source[i], redundantLength)) {
			result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
			return result;
		}

		while (i < length && gear_is_redundant(source[i])) i += 1;
		if (i >= length) break;

		if (source[i] == GEAR_PREPROCESSOR_SYMBOL) {

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
				if (!gear_resize_string(&result.result, strlen(result.result) + subLength)) {
					result.status = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;
					return result;
				}

				memcpy(&result.result[strlen(result.result)], &source[i], subLength);
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