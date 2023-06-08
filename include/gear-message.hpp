#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "gear-lexer.hpp"

namespace Gear {
	enum class MessageType {
		INFO,
		WARNING,
		ERROR
	};

	struct Message_t {
		operator std::string() const;
		MessageType	Type;
		std::string	Text;
		size_t		Position;
	};

	void PrintMessages(
		const std::vector<Message_t> messages,
		const std::string filePath,
		const std::string fileData,
		size_t realStartPosition = 0
	);
}