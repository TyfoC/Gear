#pragma once
#ifndef GEAR_ARRAY

#include <malloc.h>
#include <string.h>
#include "gear-typedefs.h"

void* gcreate_array(size_t numberOfElements, size_t elementSize, const void* dataToCopy);
void* gresize_array(void* source, size_t elementSize, size_t destNumberOfElements);
void* gappend_array(void* destination, const void* source, size_t elemSize, size_t destLength, size_t srcLength);

#endif