#pragma once

#include "gear-message.hpp"
#include "gear-file.hpp"
#include "gear-path.hpp"
#include "gear-lexer.hpp"

namespace Gear {
	struct Macro_t {
		std::string				Name;
		std::vector<Lexeme_t>	Parameters;
		std::vector<Lexeme_t>	Expression;
		bool					IsFunction;
	};

	struct PreprocessingResult_t {
		bool					WorkCompleted;
		std::string				Output;
		std::vector<Message_t>	Messages;
	};

	PreprocessingResult_t Preprocess(
		const std::string source,
		const std::string srcFileDirectoryPath,
		const std::vector<std::string> includePaths,
		std::vector<Macro_t> &macros
	);

	size_t CreateMacro(const std::string source, size_t namePosition, Macro_t &result);
	size_t GetMacroIndex(const std::vector<Macro_t> macros, const std::string name);
}