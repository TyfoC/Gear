#pragma once
#ifndef GEAR_STRING
#define GEAR_STRING

#include <string.h>
#include <malloc.h>
#include "gear-typedefs.h"

char* gear_join_strings(const char* first, const char* second);
unsigned char gear_is_operator(char value);
unsigned char gear_is_digit(char value);
unsigned char gear_is_space(char value);
unsigned char gear_is_line_break(char value);
unsigned char gear_is_redundant(char value);
unsigned char gear_is_alpha(char value);
unsigned char gear_is_letter(char value);
unsigned char gear_resize_string(char** source, size_t newLength);
unsigned char gear_append_to_string(char** destination, const char* source);
unsigned char gear_append_letters(char** destination, const char* letters, size_t count);
size_t gear_get_redundant_length(const char* source);
size_t gear_get_identifier_length(const char* source);
size_t gear_get_string_literal_length(const char* source, char limiter);

#endif