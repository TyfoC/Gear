/**
 * @file gear-string.h
 * @author github.com/TyfoC
 * @brief 
 * @version 1.0
 * @date 4 June 2023
 */
#pragma once
#ifndef GEAR_STRING
#define GEAR_STRING

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "gear-typedefs.h"
#include "gear-int.h"

gbool_t gis_space(char value);
gbool_t gis_line_break(char value);
gbool_t gis_digit(char value, unsigned char base);
gbool_t gis_operator(char value);
gbool_t gis_letter(char value);
gbool_t gis_redundant(char value);
size_t gget_line_length(const char* source, size_t lineBeginPosition);
size_t gfind_substring(const char* source, const char* substring, size_t position);
char* gcopy_string(const char* source);
char* gcopy_substring(const char* source, size_t position, size_t count);
char* gint_to_string(ptrdiff_t value, uint8_t radix);
char* gappend_string(char* destination, const char* source);
char* gappend_substring(char* destination, const char* source, size_t position, size_t count);
size_t gget_nesting_length(const char* source, size_t position, const char* leftLimiter, const char* rightLimiter);
char* gget_file_path_directory(const char* filePath);

#endif