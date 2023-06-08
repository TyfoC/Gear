#include <gear-expression.hpp>

static std::vector<Gear::ExpressionOperator_t> GearExpressionOperators = {
	{ Gear::GrammarType::NOT_OPERATOR, 1 },
	{ Gear::GrammarType::COMPLEMENT_OPERATOR, 1 },
	{ Gear::GrammarType::MUL_OPERATOR, 2 },
	{ Gear::GrammarType::DIV_OPERATOR, 2 },
	{ Gear::GrammarType::MOD_OPERATOR, 2 },
	{ Gear::GrammarType::ADD_OPERATOR, 3 },
	{ Gear::GrammarType::SUB_OPERATOR, 3 },
	{ Gear::GrammarType::LSH_OPERATOR, 4 },
	{ Gear::GrammarType::RSH_OPERATOR, 4 },
	{ Gear::GrammarType::GREATER_THAN_OPERATOR, 4 },
	{ Gear::GrammarType::LESS_THAN_OPERATOR, 4 },
	{ Gear::GrammarType::GREATER_OR_EQUAL_OPERATOR, 4 },
	{ Gear::GrammarType::LESS_OR_EQUAL_OPERATOR, 4 },
	{ Gear::GrammarType::EQUAL_TO_OPERATOR, 5 },
	{ Gear::GrammarType::NOT_EQUAL_OPERATOR, 5 },
	{ Gear::GrammarType::AND_OPERATOR, 6 },
	{ Gear::GrammarType::OR_OPERATOR, 8 },
	{ Gear::GrammarType::XOR_OPERATOR, 7 }
};

std::vector<Gear::Lexeme_t> Gear::InfixToRpnExpression(const std::vector<Lexeme_t> expression) {
	std::vector<Lexeme_t> rpnExpression;

	std::stack<Lexeme_t> localStack;
	size_t count = expression.size(), thisIndex, topIndex;
	for (size_t i = 0; i < count; i++) {
		thisIndex = GetExpressionOperatorIndex(expression[i].Type);
		switch (expression[i].Type) {
			case GrammarType::SPACE:
			case GrammarType::LINE_BREAK:
			case GrammarType::COMMENT:
				break;
			case GrammarType::HEX_LITERAL:
			case GrammarType::DEC_LITERAL:
			case GrammarType::OCT_LITERAL:
			case GrammarType::BIN_LITERAL:
			case GrammarType::IDENTIFIER:
				rpnExpression.push_back(expression[i]);
				break;
			case GrammarType::OPENING_PARENTHESIS_OPERATOR:
				localStack.push(expression[i]);
				break;
			case GrammarType::CLOSING_PARENTHESIS_OPERATOR:
				while (localStack.top().Type != GrammarType::OPENING_PARENTHESIS_OPERATOR) {
					rpnExpression.push_back(localStack.top());
					localStack.pop();
				}
				
				localStack.pop();
				break;
			default:
				while (!localStack.empty()) {
					topIndex = GetExpressionOperatorIndex(localStack.top().Type);
					if (GearExpressionOperators[thisIndex].Precendence < GearExpressionOperators[topIndex].Precendence) break;

					rpnExpression.push_back(localStack.top());
					localStack.pop();
				}

				localStack.push(expression[i]);
				break;
		}
	}

    while(!localStack.empty()){
		rpnExpression.push_back(localStack.top());
		localStack.pop();
    }

	return rpnExpression;
}

ptrdiff_t Gear::EvaluateRpnExpression(const std::vector<Lexeme_t> expression) {
	ptrdiff_t first, second;
	std::stack<ptrdiff_t> localStack;
	size_t count = expression.size();
	for (size_t i = 0; i < count; i++) {
		switch (expression[i].Type) {
			case GrammarType::HEX_LITERAL:
				localStack.push((ptrdiff_t)std::stoll(expression[i].Value.substr(2), 0, 16));
				break;
			case GrammarType::DEC_LITERAL:
				localStack.push((ptrdiff_t)std::stoll(expression[i].Value));
				break;
			case GrammarType::OCT_LITERAL:
				if (expression[i].Length >= 2 && (expression[i].Value[1] == 'o' || expression[i].Value[1] == 'O')) {
					localStack.push((ptrdiff_t)std::stoll(expression[i].Value.substr(2), 0, 8));
				}
				else localStack.push((ptrdiff_t)std::stoll(expression[i].Value.substr(1), 0, 8));
				break;
			case GrammarType::BIN_LITERAL:
				localStack.push((ptrdiff_t)std::stoll(expression[i].Value.substr(2), 0, 2));
				break;
			default:
				first = localStack.top();
				localStack.pop();

				if (expression[i].Type == GrammarType::NOT_OPERATOR) first = !first;
				else if (expression[i].Type == GrammarType::COMPLEMENT_OPERATOR) first = ~first;
				else {
					second = first;
					first = localStack.top();
					localStack.pop();

					if (expression[i].Type == GrammarType::ADD_OPERATOR) first += second;
					else if (expression[i].Type == GrammarType::SUB_OPERATOR) first -= second;
					else if (expression[i].Type == GrammarType::MUL_OPERATOR) first *= second;
					else if (expression[i].Type == GrammarType::DIV_OPERATOR) first /= second;
					else if (expression[i].Type == GrammarType::MOD_OPERATOR) first %= second;
					else if (expression[i].Type == GrammarType::LSH_OPERATOR) first <<= second;
					else if (expression[i].Type == GrammarType::RSH_OPERATOR) first >>= second;
					else if (expression[i].Type == GrammarType::AND_OPERATOR) first &= second;
					else if (expression[i].Type == GrammarType::OR_OPERATOR) first |= second;
					else if (expression[i].Type == GrammarType::XOR_OPERATOR) first ^= second;
					else if (expression[i].Type == GrammarType::LESS_THAN_OPERATOR) first = (ptrdiff_t)(first < second);
					else if (expression[i].Type == GrammarType::GREATER_THAN_OPERATOR) first = (ptrdiff_t)(first > second);
					else if (expression[i].Type == GrammarType::LESS_OR_EQUAL_OPERATOR) first = (ptrdiff_t)(first <= second);
					else if (expression[i].Type == GrammarType::GREATER_OR_EQUAL_OPERATOR) first = (ptrdiff_t)(first >= second);
					else if (expression[i].Type == GrammarType::EQUAL_TO_OPERATOR) first = (ptrdiff_t)(first == second);
					else if (expression[i].Type == GrammarType::NOT_EQUAL_OPERATOR) first = (ptrdiff_t)(first != second);
				}

				localStack.push(first);
				break;
		}
	}

	return localStack.top();
}

size_t Gear::GetExpressionOperatorIndex(GrammarType type) {
	size_t count = GearExpressionOperators.size();
	for (size_t i = 0; i < count; i++) if (GearExpressionOperators[i].Type == type) return i;
	return std::string::npos;
}

std::vector<Gear::ExpressionOperator_t> &Gear::GetExpressionOperators() {
	return GearExpressionOperators;
}