#pragma once

#include <stack>
#include <cmath>
#include "gear-lexer.hpp"

namespace Gear {
	struct ExpressionOperator_t {
		GrammarType	Type;
		size_t		Precendence;
	};

	std::vector<Lexeme_t> InfixToRpnExpression(const std::vector<Lexeme_t> expression);
	ptrdiff_t EvaluateRpnExpression(const std::vector<Lexeme_t> expression);
	size_t GetExpressionOperatorIndex(GrammarType type);
	std::vector<ExpressionOperator_t> &GetExpressionOperators();
}