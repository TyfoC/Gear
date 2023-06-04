#include <gear-status.h>

const char* GearStatusStrings[] = {
	"success",
	"no action",
	"unknown action",
	"missing file path",
	"failed to open file",
	"failed to read file",
	"failed to allocate memory",
	"missing preprocessor directive",
	"unknown preprocessor directive",
	"missing expression",
};

const size_t GearStatusStringsCount = sizeof(GearStatusStrings) / sizeof(GearStatusStrings[0]);