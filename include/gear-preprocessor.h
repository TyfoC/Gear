#pragma once
#ifndef GEAR_PREPROCESSOR
#define GEAR_PREPROCESSOR

#define GEAR_PREPROCESSOR_SYMBOL	'@'

#include "gear-string.h"

typedef struct gear_preprocessing_result_t {
	gear_status_t	status;
	char*			result;
} gear_preprocessing_result_t;

gear_preprocessing_result_t gear_preprocess(const char* source);

#endif