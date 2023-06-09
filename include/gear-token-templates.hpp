#pragma once

#include <vector>
#include "gear-token.hpp"

namespace Gear {
	struct TokenTemplate {
		size_t		Group;
		size_t		Type;
		std::string	Pattern;
	};

	std::vector<TokenTemplate> &GetTokenTemplates();
	size_t GetTokenTemplateIndex(size_t typeIndex);
}