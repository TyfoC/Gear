#pragma once
#ifndef GEAR
#define GEAR

#include <iostream>
#include <cstring>
#include "Gear-Lexer.hxx"
#include "Gear-Grammar.hxx"
#include "Gear-FileStream.hxx"
#include "Gear-Config.hxx"

#define GEAR_VERSION	"0.0.1"

namespace Gear {
	enum class Action {
		UNDEFINED,
		BUILD_LEXEME_TABLE,
	};

	enum ReturnCode {
		RETURN_CODE_SUCCESS,
		RETURN_CODE_MISSING_INCLUDE_DIRECTORY_PATH,
		RETURN_CODE_OUTPUT_FILE_PATH_ALREADY_SELECTED,
		RETURN_CODE_NO_ACTION_SELECTED,
		RETURN_CODE_FAILED_TO_OPEN_FILE,
		RETURN_CODE_FAILED_WRITE_FILE,
		RETURN_CODE_LEX_FAILED,
	};

	static const std::string DefaultOutputFileName = "output";
}

#endif