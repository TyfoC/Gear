#pragma once
#ifndef GEAR_RESULT
#define GEAR_RESULT

#include <vector>
#include "Gear-Message.hxx"

namespace Gear {
	template<typename T> struct ExecutionResult {
		bool					Completed = false;
		std::vector<Message>	Messages;
		T						Output;
	};
}

#endif