#pragma once
#ifndef GEAR_PATH
#define GEAR_PATH

#define GEAR_LINUX_PATH_LIMITER		'/'
#define GEAR_WINDOWS_PATH_LIMITER	'\\'

#include "gear-string.h"

char* gear_get_file_path_directory(const char* filePath);

#endif