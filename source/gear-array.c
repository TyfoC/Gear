#include <gear-array.h>

void* gcreate_array(size_t numberOfElements, size_t elementSize, const void* dataToCopy) {
	size_t finalSize = elementSize * numberOfElements;
	void* memory = malloc(finalSize);
	if (!memory) return NULL;

	if (dataToCopy) memcpy(memory, dataToCopy, finalSize);
	return memory;
}

void* gresize_array(void* source, size_t elementSize, size_t destNumberOfElements) {
	size_t finalSize = elementSize * destNumberOfElements;
	return !source ? malloc(finalSize) : realloc(source, finalSize);
}

void* gappend_array(void* destination, const void* source, size_t elemSize, size_t destLength, size_t srcLength) {
	size_t destSize = elemSize * destLength, srcSize = elemSize * srcLength;
	size_t finalSize = destSize + srcSize;
	void* result = destination ? realloc(destination, finalSize) : malloc(finalSize);
	if (!result) return NULL;

	memcpy((void*)((size_t)result + destSize), source, srcSize);
	return result;
}