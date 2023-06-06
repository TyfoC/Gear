#include <iostream>
#include <cstring>
#include "gear-file.hpp"
#include "gear-formatter.hpp"
#include "gear-lexer.hpp"
#include "gear-preprocessor.hpp"

enum class Action {
	UNDEFINED,
	INFO,
	HELP,
	LIST_TOKENS,
	LEX,
	PREPROCESS
};

enum class OutputFormat {
	STANDARD,
	JSON
};

struct Option_t {
	std::string	Name;
	std::string	Value;
};