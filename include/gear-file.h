#pragma once
#ifndef GEAR_FILE
#define GEAR_FILE

#include <stdio.h>
#include <stdlib.h>
#include "gear-string.h"

#define GEAR_FILE_MODE_BINARY	1
#define GEAR_FILE_MODE_TEXT		2
#define GEAR_FILE_MODE_READ		4
#define GEAR_FILE_MODE_WRITE	8
#define GEAR_FILE_IS_OPEN(...)	((__VA_ARGS__).handle != NULL)

typedef struct gear_file_t {
	char*			path;
	unsigned char	mode;
	FILE*			handle;
} gear_file_t;

gear_file_t gear_open_file(const char* path, unsigned char mode);
void gear_close_file(gear_file_t value);
size_t gear_get_file_size(const gear_file_t value);
char* gear_read_file(const gear_file_t value);

#endif