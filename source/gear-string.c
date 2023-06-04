#include <gear-string.h>

/**
 * @brief Check if the value is whitespace symbol
 * 
 * @param value
 * @return [gbool_t]
 */
gbool_t gis_space(char value) {
	return value == ' ' || value == '\t' || value == '\v' || value == '\f';
}

/**
 * @brief Check if the value is line break
 * 
 * @param value
 * @return [gbool_t]
 */
gbool_t gis_line_break(char value) {
	return value == '\r' || value == '\n';
}

/**
 * @brief Check if the value is digit
 * 
 * @param value
 * @param base	
 * @return [gbool_t]
 */
gbool_t gis_digit(char value, unsigned char base) {
	if (base <= 10) return value >= '0' && value <= ('0' + (char)(base - 1));
	if (value >= '0' && value <= '9') return TRUE;
	return (value >= 'A' && value <= ('A' + (char)(base - 11))) || (value >= 'a' && value <= ('a' + (char)(base - 11)));
}

/**
 * @brief Check if the value is operator
 * 
 * @param value
 * @return [gbool_t]
 */
gbool_t gis_operator(char value) {
	switch(value) {
		case '`':
		case '@': case '#': case '?':
		case '=':
		case '+': case '-': case '*': case '%':
		case '!': case '&': case '^': case '~':
		case '(': case ')': case '{': case '}': case '[': case ']': case '<': case '>':
		case '|': case '\\': case '/':
		case '\'': case '\"':
		case ',': case '.': case ':': case ';':
			return TRUE;
	}

	return FALSE;
}

/**
 * @brief Check if the value is letter
 * 
 * @param value 
 * @return [gbool_t] 
 */
gbool_t gis_letter(char value) {
	return !(gis_space(value) || gis_line_break(value) || gis_digit(value, 10) || gis_operator(value));
}

/**
 * @brief Check if the value is redundant (whitespace or line break)
 * 
 * @param value 
 * @return [gbool_t] 
 */
gbool_t gis_redundant(char value) {
	return gis_space(value) || gis_line_break(value);
}

size_t gget_line_length(const char* source, size_t lineBeginPosition) {
	size_t length = (size_t)strlen(source);
	if (lineBeginPosition >= length) return 0;

	size_t result = 0;
	for (; lineBeginPosition < length; lineBeginPosition++) {
		if (gis_line_break(source[lineBeginPosition])) break;
		result += 1;
	}
	
	return result;
}

size_t gfind_substring(const char* source, const char* substring, size_t position) {
	size_t length = (size_t)strlen(source), subLength = (size_t)strlen(substring);
	if (position >= length) return NPOS;

	while (position < length) {
		if (!memcmp(&source[position], substring, subLength)) return position;
		position += 1;
	}

	return NPOS;
}

char* gcopy_string(const char* source) {
	size_t length = strlen(source) + 1;
	char* result = (char*)calloc(length, sizeof(char));
	if (!result) return NULL;
	
	memcpy(result, source, length);
	return result;
}

char* gcopy_substring(const char* source, size_t position, size_t count) {
	size_t length = strlen(source);
	if (position + count > length) return NULL;

	char* result = (char*)calloc(count + 1, sizeof(char));
	if (!result) return NULL;

	memcpy(result, &source[position], count);
	result[count] = 0;
	return result;
}

char* gint_to_string(ptrdiff_t value, uint8_t radix) {
	size_t length = gget_int_digits_count(value, radix);
	char* result = (char*)calloc(length + 1, sizeof(char));
	if (!result) return NULL;

	sprintf(result, "%d", (int)value);
	return result;
}

char* gappend_string(char* destination, const char* source) {
	size_t destLen = destination ? strlen(destination) : 0, srcLen = source ? strlen(source) : 0;
	size_t finalLen = destLen + srcLen + 1;
	
	char* result;
	if (destination) result = (char*)realloc(destination, finalLen);
	else result = (char*)malloc(finalLen);
	if (!result) return NULL;

	if (source) strcat(result, source);
	return result;
}

char* gappend_substring(char* destination, const char* source, size_t position, size_t count) {
	size_t destLen = strlen(destination), srcLen = strlen(source);
	size_t finalSize = destLen + count + 1;
	if (position + count > srcLen) return NULL;

	char* result = (char*)realloc(destination, finalSize);
	if (!result) return NULL;

	memcpy(&result[destLen], &source[position], count);
	result[finalSize - 1] = 0;
	return result;
}

size_t gget_nesting_length(const char* source, size_t position, const char* leftLimiter, const char* rightLimiter) {
	size_t length = strlen(source), leftLimiterLength = strlen(leftLimiter), rightLimiterLength = strlen(rightLimiter);
	if (position >= length) return NPOS;

	size_t level = 0, count = 0;
	for (; position < length; position++, count++) {
		if (!memcmp(&source[position], leftLimiter, leftLimiterLength)) level += 1;
		else if (!memcmp(&source[position], rightLimiter, rightLimiterLength)) {
			if (!level) break;
			if (level == 1) return count + rightLimiterLength;
			level -= 1;
		}
	}

	return NPOS;
}

/**
 * @brief /path/to/file.txt -> /path/to/
 * 
 * @param filePath 
 * @return char* 
 */
char* gget_file_path_directory(const char* filePath) {
	size_t length = strlen(filePath);
	size_t limiterPos = length;
	do {
		if (filePath[limiterPos] == '/' || filePath[limiterPos] == '\\') break;
		limiterPos -= 1;
	} while (limiterPos < length);

	if (limiterPos >= length) limiterPos = 1;

	char* result = (char*)calloc(limiterPos + 2, sizeof(char));
	if (!result) return NULL;

	memcpy(result, filePath, limiterPos + 1);
	result[limiterPos + 1] = 0;
	return result;
}