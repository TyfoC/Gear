#pragma once

#include "gear-scanner.hpp"

namespace Gear {
	struct MathExpressionOperator {
		size_t	Type;
		size_t	Precendence;
	};

	std::vector<Token> MathInfixToPostfixExpression(const std::vector<Token> &tokens);
	ptrdiff_t MathEvaluatePostfixExpression(const std::vector<Token> &tokens);
	size_t MathGetExpressionOperatorIndex(size_t operatorType);
}