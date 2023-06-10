#pragma once

#include "gear-message.hpp"
#include "gear-file-utils.hpp"
#include "gear-path-utils.hpp"
#include "gear-string-utils.hpp"
#include "gear-math-expression.hpp"
#include "gear-scanner.hpp"

namespace Gear {
	struct Macro {
		std::string			Name;
		std::vector<Token>	Parameters;
		std::vector<Token>	Expression;
		bool				IsFunction;
	};

	struct PreprocessingResult {
		bool					IsWorkCompleted;
		std::string				Output;
		std::vector<Message>	Messages;
	};

	class Preprocessor {
		public:
		static PreprocessingResult Preprocess(
			const std::string source,
			const std::string filePath,
			const std::vector<std::string> includePaths,
			std::vector<Macro> &macros
		);

		static size_t GetMacroIndex(const std::vector<Macro> &macros, const std::string &name);

		static bool CreateMacro(
			Scanner &scanner,
			Macro &result
		);

		static bool ExpandExpression(
			const std::vector<Token> &expression,
			const std::vector<Token> &parameters,
			const std::vector<std::vector<Token> > &arguments,
			const std::vector<Macro> &macros,
			std::vector<Token> &result
		);
	};
}