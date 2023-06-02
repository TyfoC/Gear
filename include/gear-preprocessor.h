#pragma once
#ifndef GEAR_PREPROCESSOR
#define GEAR_PREPROCESSOR

#define GEAR_PREPROCESSOR_SYMBOL	'@'
#define GEAR_PREPROCESSOR_JUMP_TO_USEFUL($$target, $$source, $$position, $$tmpLen, $$sourceLen, $$retVal, $$retStatus) \
if (($$tmpLen = gear_get_redundant_length(&$$source[$$position]))) {\
	if (!gear_append_letters(&$$target, &$$source[$$position], $$tmpLen)) {\
		$$retStatus = GEAR_STATUS_FAILED_TO_ALLOCATE_MEMORY;\
		return $$retVal;\
	}\
	$$position += $$tmpLen;\
}
#define GEAR_PREPROCESSOR_SKIP_TO_USEFUL($$target, $$source, $$position, $$tmpLen, $$sourceLen) \
if (($$tmpLen = gear_get_redundant_length(&$$source[$$position]))) $$position += $$tmpLen;

#include "gear-string.h"

typedef struct gear_preprocessing_result_t {
	gear_status_t	status;
	char*			result;
} gear_preprocessing_result_t;

gear_preprocessing_result_t gear_preprocess(const char* source);

#endif