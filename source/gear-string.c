#include <gear-string.h>

char* gear_join_strings(const char* first, const char* second) {
	size_t firstLength = strlen(first);
	size_t secondLength = strlen(second);
	char* result = (char*)calloc(firstLength + secondLength + 1, sizeof(char));
	if (!result) return NULL;

	memcpy(result, first, firstLength);
	memcpy(&result[firstLength], second, secondLength + 1);
	return result;
}

size_t gear_find_substring(const char* source, const char* substring, size_t position) {
	size_t srcLen = strlen(source), subLen = strlen(substring);
	for (; position < srcLen; position++) if (!memcmp(&source[position], substring, subLen)) return position;
	return GEAR_NPOS;
}

size_t gear_find_char(const char* source, char character, size_t position) {
	size_t srcLen = strlen(source);
	for (; position < srcLen; position++) if (source[position] == character) return position;
	return GEAR_NPOS;
}

char* gear_get_substring(const char* source, size_t position, size_t count) {
	size_t srcLen = strlen(source);
	if (position + count > srcLen) return NULL;

	char* substring = (char*)calloc(count + 1, sizeof(char));
	if (!substring) return NULL;

	memcpy(substring, &source[position], count);
	substring[count] = 0;
	return substring;
}

char* gear_copy_string(const char* source) {
	size_t srcFullLen = strlen(source) + 1;
	char* srcCopy = (char*)calloc(srcFullLen, sizeof(char));
	if (!srcCopy) return NULL;

	memcpy(srcCopy, source, srcFullLen);
	return srcCopy;
}

unsigned char gear_is_operator(char value) {
	switch(value) {
		case '~':
		case '!':
		case '@':
		case '\"':
		case '#':
		case ';':
		case '%':
		case '^':
		case ':':
		case '&':
		case '?':
		case '*':
		case '(':
		case ')':
		case '-':
		case '=':
		case '+':
		case '{':
		case '}':
		case '[':
		case ']':
		case '\'':
		case '\\':
		case '|':
		case ',':
		case '.':
		case '<':
		case '>':
		case '/':
			return 1;
	}

	return 0;
}

unsigned char gear_is_digit(char value) {
	switch(value) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return 1;
	}
	
	return 0;
}

unsigned char gear_is_space(char value) {
	switch(value) {
		case ' ':
		case '\t':
		case '\v':
		case '\f':
			return 1;
	}

	return 0;
}

unsigned char gear_is_line_break(char value) {
	switch(value) {
		case '\r':
		case '\n':
			return 1;
	}

	return 0;
}

unsigned char gear_is_redundant(char value) {
	return gear_is_space(value) || gear_is_line_break(value);
}

unsigned char gear_is_alpha(char value) {
	return !gear_is_operator(value) && !gear_is_space(value) && !gear_is_line_break(value);
}

unsigned char gear_is_letter(char value) {
	return !gear_is_operator(value) && !gear_is_digit(value) && !gear_is_space(value) && !gear_is_line_break(value);
}

unsigned char gear_resize_string(char** source, size_t newLength) {
	return gear_resize_array((void**)source, newLength + 1);
}

unsigned char gear_append_string(char** destination, const char* source) {
	return gear_append_array((void**)destination, source, strlen(*destination), strlen(source) + 1);
}

unsigned char gear_append_letters(char** destination, const char* letters, size_t count) {
	size_t destLength = strlen(*destination);
	if (!gear_resize_array((void**)destination, destLength + count + 1)) return 0;

	memcpy(&(*destination)[destLength], letters, count);
	(*destination)[destLength + count] = 0;
	return 1;
}

size_t gear_get_redundant_length(const char* source) {
	size_t result = 0, length = strlen(source);
	while (result < length && gear_is_redundant(source[result])) result += 1;
	return result;
}

size_t gear_get_identifier_length(const char* source) {
	if (!gear_is_letter(source[0])) return 0;

	size_t result = 1, length = strlen(source);
	while (result < length && gear_is_alpha(source[result])) result += 1;
	return result;
}

size_t gear_get_string_literal_length(const char* source, char limiter) {
	if (!source || source[0] != limiter) return 0;

	size_t result = 1, length = strlen(source);
	while (result < length) {
		if (source[result] == limiter && source[result - 1] != '\\') return result + 1;
		result += 1;
	}

	return result;
}