#pragma once
#ifndef GEAR_STRING
#define GEAR_STRING

#include <string.h>
#include "gear-array.h"

char* gear_join_strings(const char* first, const char* second);
size_t gear_find_substring(const char* source, const char* substring, size_t position);
size_t gear_find_char(const char* source, char character, size_t position);
char* gear_get_substring(const char* source, size_t position, size_t count);
char* gear_copy_string(const char* source);
unsigned char gear_is_operator(char value);
unsigned char gear_is_digit(char value);
unsigned char gear_is_space(char value);
unsigned char gear_is_line_break(char value);
unsigned char gear_is_redundant(char value);
unsigned char gear_is_alpha(char value);
unsigned char gear_is_letter(char value);
unsigned char gear_resize_string(char** source, size_t newLength);
unsigned char gear_append_string(char** destination, const char* source);
unsigned char gear_append_letters(char** destination, const char* letters, size_t count);
size_t gear_get_redundant_length(const char* source);
size_t gear_get_identifier_length(const char* source);
size_t gear_get_string_literal_length(const char* source, char limiter);

#endif