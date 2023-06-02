#pragma once
#ifndef GEAR_ARRAY
#define GEAR_ARRAY

#include <string.h>
#include <malloc.h>
#include "gear-typedefs.h"

unsigned char gear_resize_array(void** source, size_t size);
unsigned char gear_append_array(void** destination, const void* source, size_t destinationLength, size_t sourceLength);

#endif