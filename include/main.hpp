#pragma once

#include <iostream>
#include <cstring>
#include "gear-version.hpp"
#include "gear-file-utils.hpp"
#include "gear-formatter.hpp"
#include "gear-scanner.hpp"
#include "gear-preprocessor.hpp"

enum Action {
	ACTION_UNDEFINED,
	ACTION_INFO,
	ACTION_HELP,
	ACTION_TOKENIZE,
	ACTION_PREPROCESS,
};

enum Format {
	FORMAT_STANDARD,
	FORMAT_JSON,
};