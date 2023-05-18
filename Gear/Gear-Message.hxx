#pragma once
#ifndef GEAR_MESSAGE
#define GEAR_MESSAGE

#include <string>

namespace Gear {
	enum MessageType {
		MESSAGE_TYPE_UNDEFINED,
		MESSAGE_TYPE_INFORMATION,
		MESSAGE_TYPE_WARNING,
		MESSAGE_TYPE_ERROR
	};

	struct Message {
		operator std::string() const;
		size_t		Type;
		std::string	Value;
	};
}

#endif