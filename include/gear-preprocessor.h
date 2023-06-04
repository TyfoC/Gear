#pragma once
#ifndef GEAR_PREPROCESSOR
#define GEAR_PREPROCESSOR

#include "gear-file.h"
#include "gear-lexer.h"

typedef struct gpreprocessing_result_t {
	gbool_t	completed_successfully;
	char*	output;
} gpreprocessing_result_t;

gpreprocessing_result_t gpreprocess(const char* source, const char* srcFileDirectoryPath);

#endif