#include <gear-scanner.hpp>

Gear::Scanner::Scanner(const std::string &source) {
	Source_ = source;
	SourceLength_ = Source_.length();
	Position_ = 0;
}

Gear::Token Gear::Scanner::Scan() {
	if (Position_ >= SourceLength_) return { Token::GROUP_EOF, Token::TYPE_EOF, std::string::npos, "", 0 };

	Token result;
	std::regex expr;
	std::cmatch exprMatch;
	size_t lexemeLength;
	std::string_view sourceView = std::string_view(Source_);
	std::string_view subStrView = sourceView.substr(Position_);
	std::vector<TokenTemplate> tokenTemplates = GetTokenTemplates();
	for (const TokenTemplate &tokenTemplate : tokenTemplates) {
		expr = std::regex(tokenTemplate.Pattern);
		if (std::regex_search(subStrView.begin(), subStrView.end(), exprMatch, expr) && !exprMatch.position()) {
			result.Group = tokenTemplate.Group;
			result.Type = tokenTemplate.Type;
			result.Position = Position_;
			result.Lexeme = exprMatch.str();
			lexemeLength = result.Lexeme.length();
			if (result.Group == Token::GROUP_LITERAL) {
				switch (result.Type) {
					case Token::TYPE_HEX_LITERAL:
						result.Literal = (void*)new ptrdiff_t((ptrdiff_t)std::stoll(result.Lexeme.substr(2), 0, 16));
						break;
					case Token::TYPE_DEC_LITERAL:
						result.Literal = (void*)new ptrdiff_t((ptrdiff_t)std::stoll(result.Lexeme));
						break;
					case Token::TYPE_OCT_LITERAL:
						if (result.Lexeme[1] == 'o' || result.Lexeme[1] == 'O') {
							result.Literal = (void*)new ptrdiff_t((ptrdiff_t)std::stoll(result.Lexeme.substr(2), 0, 8));
						}
						else result.Literal = (void*)new ptrdiff_t((ptrdiff_t)std::stoll(result.Lexeme.substr(1), 0, 8));
						break;
					case Token::TYPE_BIN_LITERAL:
						result.Literal = (void*)new ptrdiff_t((ptrdiff_t)std::stoll(result.Lexeme.substr(2), 0, 2));
						break;
					case Token::TYPE_CHAR_LITERAL:
						result.Literal = (void*)new char[lexemeLength + 1];
						memcpy(result.Literal, result.Lexeme.c_str(), lexemeLength + 1);
						break;
					case Token::TYPE_STRING_LITERAL:
						result.Literal = (void*)new char[lexemeLength + 1];
						memcpy(result.Literal, result.Lexeme.c_str(), lexemeLength + 1);
						break;
				}
			}
			else result.Literal = 0;
			return result;
		}
	}

	return { Token::GROUP_UNDEFINED, Token::TYPE_UNDEFINED, std::string::npos, "", 0 };
}

Gear::Token Gear::Scanner::ScanNext() {
	Token result = Scan();
	if (Position_ < SourceLength_) Position_ += result.Lexeme.length();
	return result;
}

bool Gear::Scanner::ScanExpression(std::vector<Gear::Token> &result) {
	if (Position_ >= SourceLength_) return false;
	size_t currentPosition = Position_;
	if (!ScanExpressionNext(result)) {
		SetPosition(currentPosition);
		return false;
	}

	SetPosition(currentPosition);
	return true;
}

bool Gear::Scanner::ScanExpressionNext(std::vector<Gear::Token> &result) {
	if (Position_ >= SourceLength_) return false;
	size_t currentPosition = Position_;

	Token token = ScanNext();
	size_t openingLimiterType = token.Type, closingLimiterType;
	if (token.Type == Token::TYPE_OPENING_PARENTHESIS_OPERATOR) closingLimiterType = Token::TYPE_CLOSING_PARENTHESIS_OPERATOR;
	else if (token.Type == Token::TYPE_OPENING_BRACKET_OPERATOR) closingLimiterType = Token::TYPE_CLOSING_BRACKET_OPERATOR;
	else if (token.Type == Token::TYPE_OPENING_BRACE_OPERATOR) closingLimiterType = Token::TYPE_CLOSING_BRACE_OPERATOR;
	else if (token.Type == Token::TYPE_LESS_THAN_OPERATOR) closingLimiterType = Token::TYPE_GREATER_THAN_OPERATOR;
	else {
		SetPosition(currentPosition);
		return false;
	}

	size_t level = 1;
	do {
		token = ScanNext();
		if (token.Group == Token::GROUP_EOF) {
			SetPosition(currentPosition);
			return false;
		}
		else if (token.Type == openingLimiterType) level += 1;
		else if (token.Type == closingLimiterType) {
			if (level == 1) break;
			level -= 1;
		}

		result.push_back(token);
	} while (true);
	return true;
}

bool Gear::Scanner::ScanSeparatedExpressions(size_t separatorType, std::vector<std::vector<Token> > &result, bool skipRedundant) {
	if (Position_ >= SourceLength_) return false;
	size_t currentPosition = Position_;
	if (!ScanSeparatedExpressionsNext(separatorType, result, skipRedundant)) {
		SetPosition(currentPosition);
		return false;
	}

	SetPosition(currentPosition);
	return true;
}

bool Gear::Scanner::ScanSeparatedExpressionsNext(size_t separatorType, std::vector<std::vector<Token> > &result, bool skipRedundant) {
	if (Position_ >= SourceLength_) return false;
	size_t currentPosition = Position_;

	Token token = ScanNext();
	size_t openingLimiterType = token.Type, closingLimiterType;
	if (token.Type == Token::TYPE_OPENING_PARENTHESIS_OPERATOR) closingLimiterType = Token::TYPE_CLOSING_PARENTHESIS_OPERATOR;
	else if (token.Type == Token::TYPE_OPENING_BRACKET_OPERATOR) closingLimiterType = Token::TYPE_CLOSING_BRACKET_OPERATOR;
	else if (token.Type == Token::TYPE_OPENING_BRACE_OPERATOR) closingLimiterType = Token::TYPE_CLOSING_BRACE_OPERATOR;
	else if (token.Type == Token::TYPE_LESS_THAN_OPERATOR) closingLimiterType = Token::TYPE_GREATER_THAN_OPERATOR;
	else {
		SetPosition(currentPosition);
		return false;
	}

	size_t level = 1;
	std::vector<Token> tmp;
	size_t prevType = Token::TYPE_UNDEFINED;
	do {
		token = ScanNext();
		if (token.Group == Token::GROUP_EOF) return false;
		else if (token.Type == openingLimiterType) {
			level += 1;
			prevType = token.Type;
			tmp.push_back(token);
		}
		else if (token.Type == closingLimiterType) {
			if (level == 1) {
				if (tmp.empty()) return false;
				result.push_back(tmp);
				break;
			}

			level -= 1;
			prevType = token.Type;
			tmp.push_back(token);
		}
		else if (token.Type == separatorType) {
			if (prevType == separatorType) return false;
			prevType = token.Type;
			result.push_back(tmp);
			tmp.clear();
		}
		else {
			if (token.Group != Token::GROUP_REDUNDANT) prevType = token.Type;
			if (skipRedundant) {
				if (token.Group != Token::GROUP_REDUNDANT) tmp.push_back(token);
			}
			else if (token.Type != Token::TYPE_COMMENT) tmp.push_back(token);
		}
	} while (true);
	return true;
}

std::vector<Gear::Token> Gear::Scanner::ScanToEnd() {
	std::vector<Token> result;
	while (Position_ < SourceLength_) result.push_back(ScanNext());
	return result;
}

size_t Gear::Scanner::GetPosition() const {
	return Position_;
}

size_t Gear::Scanner::GetSourceLength() const {
	return SourceLength_;
}

void Gear::Scanner::SetPosition(size_t position) {
	Position_ = position;
}

bool Gear::Scanner::IsAtEnd() const {
	return Position_ >= SourceLength_;
}

bool Gear::Scanner::ScanSeparatedExpressions(
	const std::vector<Token> &tokens,
	size_t index,
	size_t separatorType,
	std::vector<std::vector<Token> > &result,
	bool skipRedundant
) {
	size_t count = tokens.size();
	if (index >= count) return false;

	size_t openingLimiterType = tokens[index].Type, closingLimiterType;
	if (tokens[index].Type == Token::TYPE_OPENING_PARENTHESIS_OPERATOR) closingLimiterType = Token::TYPE_CLOSING_PARENTHESIS_OPERATOR;
	else if (tokens[index].Type == Token::TYPE_OPENING_BRACKET_OPERATOR) closingLimiterType = Token::TYPE_CLOSING_BRACKET_OPERATOR;
	else if (tokens[index].Type == Token::TYPE_OPENING_BRACE_OPERATOR) closingLimiterType = Token::TYPE_CLOSING_BRACE_OPERATOR;
	else if (tokens[index].Type == Token::TYPE_LESS_THAN_OPERATOR) closingLimiterType = Token::TYPE_GREATER_THAN_OPERATOR;
	else return false;

	index += 1;
	size_t level = 1;
	std::vector<Token> tmp;
	size_t prevType = Token::TYPE_UNDEFINED;
	while (index < count) {
		if (tokens[index].Group == Token::GROUP_EOF) return false;
		else if (tokens[index].Type == openingLimiterType) {
			level += 1;
			prevType = tokens[index].Type;
			tmp.push_back(tokens[index]);
		}
		else if (tokens[index].Type == closingLimiterType) {
			if (level == 1) {
				if (tmp.empty()) return false;
				result.push_back(tmp);
				break;
			}

			level -= 1;
			prevType = tokens[index].Type;
			tmp.push_back(tokens[index]);
		}
		else if (tokens[index].Type == separatorType) {
			if (prevType == separatorType) return false;
			prevType = tokens[index].Type;
			result.push_back(tmp);
			tmp.clear();
		}
		else {
			if (tokens[index].Group != Token::GROUP_REDUNDANT) prevType = tokens[index].Type;
			if (skipRedundant) {
				if (tokens[index].Group != Token::GROUP_REDUNDANT) tmp.push_back(tokens[index]);
			}
			else if (tokens[index].Type != Token::TYPE_COMMENT) tmp.push_back(tokens[index]);
		}

		index += 1;
	}
	return true;
}