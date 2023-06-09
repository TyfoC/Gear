#include <gear-math-expression.hpp>

static std::vector<Gear::MathExpressionOperator> GearMathExpressionOperators = {
	{ Gear::Token::TYPE_NOT_OPERATOR, 1 },
	{ Gear::Token::TYPE_COMPLEMENT_OPERATOR, 1 },
	{ Gear::Token::TYPE_MUL_OPERATOR, 2 },
	{ Gear::Token::TYPE_DIV_OPERATOR, 2 },
	{ Gear::Token::TYPE_MOD_OPERATOR, 2 },
	{ Gear::Token::TYPE_ADD_OPERATOR, 3 },
	{ Gear::Token::TYPE_SUB_OPERATOR, 3 },
	{ Gear::Token::TYPE_LSH_OPERATOR, 4 },
	{ Gear::Token::TYPE_RSH_OPERATOR, 4 },
	{ Gear::Token::TYPE_GREATER_THAN_OPERATOR, 4 },
	{ Gear::Token::TYPE_LESS_THAN_OPERATOR, 4 },
	{ Gear::Token::TYPE_GREATER_OR_EQUAL_OPERATOR, 4 },
	{ Gear::Token::TYPE_LESS_OR_EQUAL_OPERATOR, 4 },
	{ Gear::Token::TYPE_EQUAL_TO_OPERATOR, 5 },
	{ Gear::Token::TYPE_NOT_EQUAL_OPERATOR, 5 },
	{ Gear::Token::TYPE_AND_OPERATOR, 6 },
	{ Gear::Token::TYPE_OR_OPERATOR, 8 },
	{ Gear::Token::TYPE_XOR_OPERATOR, 7 }
};

std::vector<Gear::Token> Gear::MathInfixToPostfixExpression(const std::vector<Token> &tokens) {
	std::vector<Token> postfixExpression;

	std::stack<Token> localStack;
	size_t count = tokens.size(), thisIndex, topIndex;
	for (size_t i = 0; i < count; i++) {
		thisIndex = MathGetExpressionOperatorIndex(tokens[i].Type);
		switch (tokens[i].Type) {
			case Token::TYPE_SPACE:
			case Token::TYPE_LINE_BREAK:
			case Token::TYPE_COMMENT:
				break;
			case Token::TYPE_HEX_LITERAL:
			case Token::TYPE_DEC_LITERAL:
			case Token::TYPE_OCT_LITERAL:
			case Token::TYPE_BIN_LITERAL:
			case Token::TYPE_IDENTIFIER:
				postfixExpression.push_back(tokens[i]);
				break;
			case Token::TYPE_OPENING_PARENTHESIS_OPERATOR:
				localStack.push(tokens[i]);
				break;
			case Token::TYPE_CLOSING_PARENTHESIS_OPERATOR:
				while (localStack.top().Type != Token::TYPE_OPENING_PARENTHESIS_OPERATOR) {
					postfixExpression.push_back(localStack.top());
					localStack.pop();
				}
				
				localStack.pop();
				break;
			default:
				while (!localStack.empty()) {
					topIndex = MathGetExpressionOperatorIndex(localStack.top().Type);
					if (GearMathExpressionOperators[thisIndex].Precendence < GearMathExpressionOperators[topIndex].Precendence) break;

					postfixExpression.push_back(localStack.top());
					localStack.pop();
				}

				localStack.push(tokens[i]);
				break;
		}
	}

    while(!localStack.empty()){
		postfixExpression.push_back(localStack.top());
		localStack.pop();
    }

	return postfixExpression;
}

ptrdiff_t Gear::MathEvaluatePostfixExpression(const std::vector<Token> &tokens) {
	ptrdiff_t first, second;
	std::stack<ptrdiff_t> localStack;
	size_t count = tokens.size();
	for (size_t i = 0; i < count; i++) {
		switch (tokens[i].Type) {
			case Token::TYPE_HEX_LITERAL:
				localStack.push(*(ptrdiff_t*)tokens[i].Literal);
				break;
			case Token::TYPE_DEC_LITERAL:
				localStack.push(*(ptrdiff_t*)tokens[i].Literal);
				break;
			case Token::TYPE_OCT_LITERAL:
				localStack.push(*(ptrdiff_t*)tokens[i].Literal);
				break;
			case Token::TYPE_BIN_LITERAL:
				localStack.push(*(ptrdiff_t*)tokens[i].Literal);
				break;
			default:
				first = localStack.top();
				localStack.pop();

				if (tokens[i].Type == Token::TYPE_NOT_OPERATOR) first = !first;
				else if (tokens[i].Type == Token::TYPE_COMPLEMENT_OPERATOR) first = ~first;
				else {
					second = first;
					first = localStack.top();
					localStack.pop();

					if (tokens[i].Type == Token::TYPE_ADD_OPERATOR) first += second;
					else if (tokens[i].Type == Token::TYPE_SUB_OPERATOR) first -= second;
					else if (tokens[i].Type == Token::TYPE_MUL_OPERATOR) first *= second;
					else if (tokens[i].Type == Token::TYPE_DIV_OPERATOR) first /= second;
					else if (tokens[i].Type == Token::TYPE_MOD_OPERATOR) first %= second;
					else if (tokens[i].Type == Token::TYPE_LSH_OPERATOR) first <<= second;
					else if (tokens[i].Type == Token::TYPE_RSH_OPERATOR) first >>= second;
					else if (tokens[i].Type == Token::TYPE_AND_OPERATOR) first &= second;
					else if (tokens[i].Type == Token::TYPE_OR_OPERATOR) first |= second;
					else if (tokens[i].Type == Token::TYPE_XOR_OPERATOR) first ^= second;
					else if (tokens[i].Type == Token::TYPE_LESS_THAN_OPERATOR) first = (ptrdiff_t)(first < second);
					else if (tokens[i].Type == Token::TYPE_GREATER_THAN_OPERATOR) first = (ptrdiff_t)(first > second);
					else if (tokens[i].Type == Token::TYPE_LESS_OR_EQUAL_OPERATOR) first = (ptrdiff_t)(first <= second);
					else if (tokens[i].Type == Token::TYPE_GREATER_OR_EQUAL_OPERATOR) first = (ptrdiff_t)(first >= second);
					else if (tokens[i].Type == Token::TYPE_EQUAL_TO_OPERATOR) first = (ptrdiff_t)(first == second);
					else if (tokens[i].Type == Token::TYPE_NOT_EQUAL_OPERATOR) first = (ptrdiff_t)(first != second);
				}

				localStack.push(first);
				break;
		}
	}

	return localStack.top();
}

size_t Gear::MathGetExpressionOperatorIndex(size_t operatorType) {
	size_t count = GearMathExpressionOperators.size();
	for (size_t i = 0; i < count; i++) if (GearMathExpressionOperators[i].Type == operatorType) return i;
	return std::string::npos;
}