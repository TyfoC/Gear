#pragma once

#include "gear-message.hpp"
#include "gear-file.hpp"
#include "gear-path.hpp"
#include "gear-lexer.hpp"

namespace Gear {
	struct PreprocessingResult_t {
		bool					WorkCompleted;
		std::string				Output;
		std::vector<Message_t>	Messages;
	};

	PreprocessingResult_t Preprocess(
		const std::string source,
		const std::string srcFileDirectoryPath,
		const std::vector<std::string> includePaths
	);
}