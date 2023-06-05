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

typedef struct gfile_t {
	char*	path;
	uint8_t	mode;
	FILE*	handle;
} gfile_t;

gfile_t gopen_file(const char* path, uint8_t mode);
void gclose_file(gfile_t* value);
size_t gget_file_size(const gfile_t value);
char* gread_file(const gfile_t value);
gbool_t gwrite_file_string(const gfile_t value, const char* source);

#endif