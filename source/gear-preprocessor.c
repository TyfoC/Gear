#include <gear-preprocessor.h>

gpreprocessing_result_t gpreprocess(const char* source, const char* srcFileDirectoryPath) {
	gpreprocessing_result_t result = { TRUE, (char*)malloc(1) };
	result.output[0] = 0;

	size_t length = strlen(source);
	glexeme_t lexeme;
	char* tmp;
	for (size_t position = 0; position < length; position += lexeme.length) {
		if (!gget_lexeme(source, position, &lexeme)) {
			if (result.output) free(result.output);
			result.completed_successfully = FALSE;
			break;
		}

		if (lexeme.type == GEAR_GRAMMAR_TYPE_OPERATOR_PREPROCESSOR_DIRECTIVE) {
			if ((position += lexeme.length) >= length) {
				result.completed_successfully = FALSE;
				break;
			}

			do {
				if (!gget_lexeme(source, position, &lexeme)) {
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				if (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT) position += lexeme.length;
			} while (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT);

			if (lexeme.group != GEAR_GRAMMAR_GROUP_IDENTIFIER) {
				if (result.output) free(result.output);
				result.completed_successfully = FALSE;
				return result;
			}

			if (lexeme.length == 3 && !memcmp(&source[position], "dog", lexeme.length)) {
				tmp = (char*)gappend_string(result.output, "@");
				if ((position += lexeme.length) >= length) break;

				if (source[position] == '@') {
					if ((position += 1) >= length) break;
					do {
						if (!gget_lexeme(source, position, &lexeme)) {
							if (result.output) free(result.output);
							result.completed_successfully = FALSE;
							return result;
						}

						if (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT) position += lexeme.length;
					} while (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT);
					lexeme.length = 0;
				}
			}
			else if (lexeme.length == 4 && !memcmp(&source[position], "join", lexeme.length)) {
				if ((position += lexeme.length) >= length) break;
				do {
					if (!gget_lexeme(source, position, &lexeme)) {
						if (result.output) free(result.output);
						result.completed_successfully = FALSE;
						return result;
					}

					if (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT) position += lexeme.length;
				} while (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT);
				lexeme.length = 0;
			}
			else if (
				lexeme.length == 6 && (
				!memcmp(&source[position], "preinc", lexeme.length) || !memcmp(&source[position], "rawinc", lexeme.length)
			)) {
				gbool_t preprocessFileBeforeAppend = source[position] == 'p';
				if ((position += lexeme.length) >= length) {
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}
				
				do {
					if (!gget_lexeme(source, position, &lexeme)) {
						if (result.output) free(result.output);
						result.completed_successfully = FALSE;
						return result;
					}

					if (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT) position += lexeme.length;
				} while (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT);

				if (lexeme.type != GEAR_GRAMMAR_TYPE_STRING_LITERAL) {
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				tmp = gcopy_string(srcFileDirectoryPath);
				if (!tmp) {
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				char* filePath = gappend_substring(tmp, source, position + 1, lexeme.length - 2);
				if (!filePath) {
					free(tmp);
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				gfile_t handle = gopen_file(filePath, GEAR_FILE_MODE_READ | GEAR_FILE_MODE_TEXT);
				if (!GEAR_FILE_IS_OPEN(handle)) {
					free(filePath);
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				char* fileData = gread_file(handle);
				if (!fileData) {
					gclose_file(handle);
					free(filePath);
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				gclose_file(handle);

				if (preprocessFileBeforeAppend) {
					char* subFileDirPath = gget_file_path_directory(filePath);
					if (!subFileDirPath) {
						free(fileData);
						free(filePath);
						if (result.output) free(result.output);
						result.completed_successfully = FALSE;
						return result;
					}

					gpreprocessing_result_t subResult = gpreprocess(fileData, subFileDirPath);
					if (!subResult.completed_successfully) {
						free(subFileDirPath);
						free(fileData);
						free(filePath);
						if (result.output) free(result.output);
						result.completed_successfully = FALSE;
						return result;
					}

					tmp = gappend_string(result.output, subResult.output);
					free(subResult.output);
					free(subFileDirPath);
				}
				else tmp = gappend_string(result.output, fileData);

				if (!tmp) {
					free(fileData);
					free(filePath);
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				free(fileData);
				free(filePath);

				result.output = tmp;
			}
			else if (lexeme.length == 4 && !memcmp(&source[position], "save", lexeme.length)) {
				if ((position += lexeme.length) >= length) break;
				do {
					if (!gget_lexeme(source, position, &lexeme)) {
						if (result.output) free(result.output);
						result.completed_successfully = FALSE;
						return result;
					}

					if (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT) position += lexeme.length;
				} while (lexeme.group == GEAR_GRAMMAR_GROUP_REDUNDANT);

				if (lexeme.type != GEAR_GRAMMAR_TYPE_OPERATOR_OPENING_BRACE) {
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				size_t indexRightGrammar = gfind_grammar_index_by_type(GEAR_GRAMMAR_TYPE_OPERATOR_CLOSING_BRACE);
				if (indexRightGrammar == NPOS) {
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				char* leftLimiter = gcopy_substring(source, position, lexeme.length);
				if (!leftLimiter) {
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				char* rightLimiter = gcopy_string(GearGrammar[indexRightGrammar].pattern);
				if (!rightLimiter) {
					free(leftLimiter);
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				size_t nestingLength = gget_nesting_length(
					source,
					position,
					leftLimiter,
					rightLimiter
				);

				free(leftLimiter);
				free(rightLimiter);

				if (nestingLength == NPOS) {
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				tmp = (char*)gappend_substring(result.output, source, position + 1, nestingLength - 2);
				if (!tmp) {
					if (result.output) free(result.output);
					result.completed_successfully = FALSE;
					return result;
				}

				result.output = tmp;
				position += nestingLength;
				lexeme.length = 0;
			}
			else {
				if (result.output) free(result.output);
				result.completed_successfully = FALSE;
				return result;
			}
		}
		else if (lexeme.group == GEAR_GRAMMAR_GROUP_IDENTIFIER) {
			tmp = (char*)gappend_substring(result.output, source, position, lexeme.length);
			if (!tmp) {
				if (result.output) free(result.output);
				result.completed_successfully = FALSE;
				break;
			}

			result.output = tmp;
		}
		else {
			if (lexeme.type == GEAR_GRAMMAR_TYPE_SINGLE_LINE_COMMENT || lexeme.type == GEAR_GRAMMAR_TYPE_MULTILINE_COMMENT) continue;

			tmp = (char*)gappend_substring(result.output, source, position, lexeme.length);
			if (!tmp) {
				if (result.output) free(result.output);
				result.completed_successfully = FALSE;
				break;
			}

			result.output = tmp;
		}
	}

	return result;
}