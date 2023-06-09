#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "gear-token.hpp"
#include "gear-string-utils.hpp"

namespace Gear {
	struct Message {
		enum {
			TYPE_INFO,
			TYPE_WARNING,
			TYPE_ERROR
		};

		operator std::string() const;
		size_t		Type;
		std::string	Text;
		size_t		Position;
	};

	void PrintMessages(
		const std::vector<Message> messages,
		const std::string filePath,
		const std::string fileData,
		size_t realStartPosition = 0
	);
}