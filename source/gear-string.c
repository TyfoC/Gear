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
	if (!*source) {
		*source = (char*)calloc(1, sizeof(char));
		if (!*source) return 0;
		*source[0] = 0;
	}

	char* result = (char*)realloc(*source, (newLength + 1) * sizeof(char));
	if (!result) return 0;

	result[newLength] = 0;
	*source = result;
	return 1;
}

unsigned char gear_append_to_string(char** destination, const char* source) {
	size_t destLength = strlen(*destination);
	if (!gear_resize_string(destination, destLength + strlen(source))) return 0;

	memcpy(&(*destination)[destLength], source, strlen(source) + 1);
	return 1;
}

unsigned char gear_append_letters(char** destination, const char* letters, size_t count) {
	size_t destLength = strlen(*destination);
	if (!gear_resize_string(destination, destLength + count)) return 0;

	memcpy(&(*destination)[destLength], letters, count);
	(*destination)[destLength + count] = 0;
	return 1;
}

size_t gear_get_redundant_length(const char* source) {
	size_t result = 0;
	while (source[result] && gear_is_redundant(source[result])) result += 1;
	return result;
}

size_t gear_get_identifier_length(const char* source) {
	if (!gear_is_letter(source[0])) return 0;

	size_t result = 1;
	while (source[result] && gear_is_alpha(source[result])) result += 1;
	return result;
}

size_t gear_get_string_literal_length(const char* source, char limiter) {
	if (source[0] != limiter) return 0;

	size_t result = 1;
	while (source[result]) {
		if (source[result] == limiter && source[result - 1] != '\\') return result + 1;
		result += 1;
	}

	return result;
}