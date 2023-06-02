#include <gear-array.h>

unsigned char gear_resize_array(void** source, size_t size) {
	void* tmp = !(*source) ? malloc(size) : realloc(*source, size);
	if (!tmp) return 0;
	
	*source = tmp;
	return 1;
}

unsigned char gear_append_array(void** destination, const void* source, size_t destinationLength, size_t sourceLength) {
	size_t resultLength = destinationLength + sourceLength;
	if (!gear_resize_array(destination, resultLength)) return 0;
	
	memcpy(&((char*)(*destination))[destinationLength], source, sourceLength);
	return 1;
}