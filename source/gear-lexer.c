#include "gear-lexer.h"

/**
 * @brief Get a lexeme from a string by position
 * @param source
 * @param position
 * @param result	lexeme
 * @return [gbool_t] is the type of the lexeme determined
 */
gbool_t gget_lexeme(const char* source, size_t position, glexeme_t* result) {
	size_t length = strlen(source);
	if (position >= length) return FALSE;

	result->position = position;
	if (gis_redundant(source[position])) {
		result->group = GEAR_GRAMMAR_GROUP_REDUNDANT;
		result->type = gis_space(source[position]) ? GEAR_GRAMMAR_TYPE_SPACE : GEAR_GRAMMAR_TYPE_LINE_BREAK;
		if (result->type == GEAR_GRAMMAR_TYPE_LINE_BREAK) {
			if (source[position] == '\r') result->length = source[position + 1] == '\n' ? 2 : 1;
			else result->length = 1;
		}
		else result->length = 1;

		return TRUE;
	}
	else if (gis_digit(source[position], 10)) {
		uint8_t radix;
		result->group = GEAR_GRAMMAR_GROUP_NUMERIC_LITERAL;
		result->length = 0;
		if (source[position] == '0') {
			position += 1;
			if (source[position] == 'x' || source[position] == 'X') {		//	Hex: 0x.../0X...
				radix = 16;
				result->type = GEAR_GRAMMAR_TYPE_HEX_LITERAL;
				result->length = 2;
			}
			else if (source[position] == 'o' || source[position] == 'O') {	//	Oct: 0o.../0O...
				radix = 8;
				result->type = GEAR_GRAMMAR_TYPE_OCT_LITERAL;
				result->length = 2;
			}
			else if (source[position] == 'b' || source[position] == 'B') {	//	Bin: 0b.../0B...
				radix = 2;
				result->type = GEAR_GRAMMAR_TYPE_BIN_LITERAL;
				result->length = 2;
			}
			else {
				if (gis_digit(source[position], 8)) {						//	Oct: 0...
					radix = 8;
					result->length = 2;
				}
				else if (!source[position] || gis_redundant(source[position]) || gis_operator(source[position])) {	//	Dec: 0
					result->type = GEAR_GRAMMAR_TYPE_DEC_LITERAL;
					result->length = 1;
					return TRUE;
				}
				else return FALSE;											//	unknown radix/unresolved symbols
			}
			position += 1;
		}
		else {
			radix = 10;
			result->type = GEAR_GRAMMAR_TYPE_DEC_LITERAL;
		}

		while (position < length) {
			if (gis_digit(source[position], radix)) result->length += 1;
			else {
				if (source[position] == '\'') {
					if (source[position + 1] == '\'') return FALSE;			//	extra separator
					result->length += 1;
				}
				else if (gis_redundant(source[position]) || gis_operator(source[position])) break;
				else return FALSE;											//	unresolved symbol
			}

			position += 1;
		}

		return TRUE;
	}
	else if (gis_operator(source[position])) {
		result->group = GEAR_GRAMMAR_GROUP_OPERATOR;
		result->length = 1;

		for (size_t i = 0; i < GearGrammarCount; i++) {
			if (GearGrammar[i].pattern[0] == '1') {							//	special check
				if (GearGrammar[i].type == GEAR_GRAMMAR_TYPE_SINGLE_LINE_COMMENT) {
					if (memcmp(&source[position], &GearGrammar[i].pattern[1], strlen(&GearGrammar[i].pattern[1]))) continue;
					result->type = GEAR_GRAMMAR_TYPE_SINGLE_LINE_COMMENT;
					result->length = gget_line_length(source, position);
					return TRUE;
				}

				size_t separartorPosition = 1;
				while (GearGrammar[i].pattern[separartorPosition]) {
					if (GearGrammar[i].pattern[separartorPosition] == '0') break;
					separartorPosition += 1;
				}

				if (memcmp(&source[position], &GearGrammar[i].pattern[1], separartorPosition - 1)) continue;
				result->type = GearGrammar[i].type;

				if (GearGrammar[i].group == GEAR_GRAMMAR_GROUP_STRING_LITERAL) {
					size_t limiterLength = separartorPosition - 1;
					if (memcmp(&source[position], &GearGrammar[i].pattern[1], limiterLength)) continue;

					size_t escapeSeparatorLength = strlen(&GearGrammar[i].pattern[separartorPosition + 1]);
					result->length = limiterLength;

					for (position += limiterLength; source[position]; position++, result->length++) {
						if (!memcmp(&source[position], &GearGrammar[i].pattern[1], limiterLength)) {
							if (position < escapeSeparatorLength || !memcmp(
								&source[position - escapeSeparatorLength],
								&GearGrammar[i].pattern[separartorPosition + 1],
								escapeSeparatorLength)
							) continue;
							result->length += limiterLength;
							break;
						}
					}

					return TRUE;
				}

				result->length = gfind_substring(source, &GearGrammar[i].pattern[separartorPosition + 1], position);
				if (result->length == NPOS) result->length = strlen(&source[position]);
				else result->length += strlen(&GearGrammar[i].pattern[separartorPosition + 1]) - position;
				return TRUE;
			}
			else if (GearGrammar[i].group == GEAR_GRAMMAR_GROUP_OPERATOR) {
				size_t patternLength = strlen(GearGrammar[i].pattern);
				if (memcmp(&source[position], GearGrammar[i].pattern, patternLength)) continue;

				result->type = GearGrammar[i].type;
				result->length = patternLength;
				return TRUE;
			}
		}
	}
	else if (gis_letter(source[position])) {
		result->group = GEAR_GRAMMAR_GROUP_IDENTIFIER;
		result->type = 0;
		result->length = 1;
		position += 1;
		while (source[position] && (gis_digit(source[position], 10) || gis_letter(source[position]))) {
			position += 1;
			result->length += 1;
		}

		return TRUE;
	}

	return FALSE;
}

size_t glex(const char* source, glexeme_t** result) {
	gbool_t emptyInput = !*result;
	if (emptyInput) {
		*result = (glexeme_t*)gcreate_array(0, sizeof(glexeme_t), NULL);
		if (!*result) return FALSE;
	}

	glexeme_t tmp;
	size_t length = (size_t)strlen(source), position = 0, count = 0;
	while (position < length) {
		if (!gget_lexeme(source, position, &tmp)) {
			if (emptyInput) free(*result);
			return NPOS;
		}

		glexeme_t* tmpArray = gappend_array(*result, &tmp, sizeof(glexeme_t), count, 1);
		if (!tmpArray) {
			if (emptyInput) free(*result);
			return NPOS;
		}

		*result = tmpArray;
		position += tmp.length;
		count += 1;
	}

	return count;
}