#include <gear-int.h>

size_t gget_int_digits_count(ptrdiff_t value, uint8_t radix) {
	size_t intLen = value < 0 ? 2 : 1;
	while (value) value /= (ptrdiff_t)radix;
	return intLen;
}