#include <gear-lexer.hpp>

static std::vector<Gear::Token_t> GearGrammar = {
	{ Gear::GrammarGroup::REDUNDANT, Gear::GrammarType::SPACE, R"([ \t\v\f])" },
	{ Gear::GrammarGroup::REDUNDANT, Gear::GrammarType::LINE_BREAK, R"((\r\n|\r|\n))" },
	{ Gear::GrammarGroup::REDUNDANT, Gear::GrammarType::COMMENT, R"((\/\*([\s\S]*?)\*\/|\/\/.*))" },
	{ Gear::GrammarGroup::LITERAL, Gear::GrammarType::HEX_LITERAL, R"(0[xX][\da-fA-F']+)" },
	{ Gear::GrammarGroup::LITERAL, Gear::GrammarType::DEC_LITERAL, R"(\d+\b)" },
	{ Gear::GrammarGroup::LITERAL, Gear::GrammarType::OCT_LITERAL, R"(0[oO]?[0-7']+\b)" },
	{ Gear::GrammarGroup::LITERAL, Gear::GrammarType::BIN_LITERAL, R"(0[bB][01]+\b)" },
	{ Gear::GrammarGroup::LITERAL, Gear::GrammarType::CHAR_LITERAL, R"('(\\[\s\S]|[^\'])*')" },
	{ Gear::GrammarGroup::LITERAL, Gear::GrammarType::STRING_LITERAL, R"("(\\[\s\S]|[^\"])*")" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::ELLIPSIS_OPERATOR, R"(\.\.\.)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::INC_OPERATOR, R"(\+\+)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::DEC_OPERATOR, R"(\-\-)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::LSH_ASSIGN_OPERATOR, R"(<<=)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::RSH_ASSIGN_OPERATOR, R"(>>=)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::EQUAL_TO_OPERATOR, R"(==)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::NOT_EQUAL_OPERATOR, R"(!=)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::GREATER_OR_EQUAL_OPERATOR, R"(>=)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::LESS_OR_EQUAL_OPERATOR, R"(<=)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::LSH_OPERATOR, R"(<<)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::RSH_OPERATOR, R"(>>)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::AND_ASSIGN_OPERATOR, R"(&=)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::OR_ASSIGN_OPERATOR, R"(\|=)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::XOR_ASSIGN_OPERATOR, R"(\^=)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::ADD_OPERATOR, R"(\+)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::SUB_OPERATOR, R"(\-)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::MUL_OPERATOR, R"(\*)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::DIV_OPERATOR, R"(\/)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::MOD_OPERATOR, R"(%)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::GREATER_THAN_OPERATOR, R"(>)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::LESS_THAN_OPERATOR, R"(<)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::NOT_OPERATOR, R"(!)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::AND_OPERATOR, R"(&)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::OR_OPERATOR, R"(\|)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::XOR_OPERATOR, R"(\^)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::COMPLEMENT_OPERATOR, R"(~)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::ASSIGN_OPERATOR, R"(=)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::OPENING_PARENTHESIS_OPERATOR, R"(\()" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::CLOSING_PARENTHESIS_OPERATOR, R"(\))" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::OPENING_BRACKET_OPERATOR, R"(\[)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::CLOSING_BRACKET_OPERATOR, R"(\])" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::OPENING_BRACE_OPERATOR, R"(\{)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::CLOSING_BRACE_OPERATOR, R"(\})" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::DOT_OPERATOR, R"(\.)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::COMMA_OPERATOR, R"(,)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::COLON_OPERATOR, R"(\:)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::SEMICOLON_OPERATOR, R"(;)" },
	{ Gear::GrammarGroup::OPERATOR, Gear::GrammarType::PREPROCESSOR_OPERATOR, R"(@)" },
	{ Gear::GrammarGroup::IDENTIFIER, Gear::GrammarType::IDENTIFIER, R"([_a-zA-Z]\w*\b)" }
};

std::vector<Gear::Lexeme_t> Gear::Lex(const std::string source) {
	std::vector<Lexeme_t> result;
	Lexeme_t lexeme;
	size_t length = source.length();
	for (size_t i = 0; i < length;) {
		lexeme = GetLexeme(source, i);
		result.push_back(lexeme);
		i += lexeme.Length;
	}
	return result;
}

Gear::Lexeme_t Gear::GetLexeme(const std::string source, size_t position) {
	std::string_view srcView = std::string_view(source);
	std::string_view strView = srcView.substr(position);

	std::regex expr;
	std::cmatch exprMatch;
	for (const Token_t &token : GearGrammar) {
		expr = std::regex(token.Pattern);
		if (std::regex_search(strView.begin(), strView.end(), exprMatch, expr) && !exprMatch.position()) return {
			token.Group, token.Type, position, exprMatch.str().length()
		};
	}

	return { GrammarGroup::UNDEFINED, GrammarType::UNDEFINED, position, 1 };
}

std::vector<Gear::Token_t> &Gear::GetGrammar() {
	return GearGrammar;
}

size_t Gear::FindTokenIndex(GrammarType grammarType) {
	size_t count = GearGrammar.size();
	for (size_t i = 0; i < count; i++) if (GearGrammar[i].Type == grammarType) return i;
	return std::string::npos;
}

size_t Gear::GetNestingLength(
	const std::string source,
	size_t position,
	GrammarType leftLimiterType,
	GrammarType rightLimiterType,
	bool avoidComments
) {
	size_t level = 0;
	Lexeme_t lexeme;
	size_t length = source.length(), result = 0;
	while (position < length) {
		lexeme = GetLexeme(source, position);
		if (lexeme.Type == leftLimiterType) level += 1;
		else if (lexeme.Type == rightLimiterType) {
			if (!level) return std::string::npos;
			else if (level == 1) return result + lexeme.Length;
			level -= 1;
		}
		else if (lexeme.Type == GrammarType::COMMENT && avoidComments) lexeme.Length = 1;	//	avoiding errors due to comments
		
		result += lexeme.Length;
		position += lexeme.Length;
	}

	return std::string::npos;
}

size_t Gear::GetNestedExpressionLength(
	const std::string source,
	size_t position,
	GrammarType leftLimiterType,
	GrammarType rightLimiterType,
	bool avoidComments
) {
	size_t length = source.length();
	if (position >= length) return std::string::npos;

	Lexeme_t lexeme = GetLexeme(source, position);
	if (lexeme.Type != leftLimiterType) return std::string::npos;

	position += lexeme.Length;

	size_t level = 1;
	size_t result = 0;
	while (position < length) {
		lexeme = GetLexeme(source, position);
		if (lexeme.Type == leftLimiterType) level += 1;
		else if (lexeme.Type == rightLimiterType) {
			if (!level) return std::string::npos;
			else if (level == 1) return result;
			level -= 1;
		}
		else if (lexeme.Type == GrammarType::COMMENT && avoidComments) lexeme.Length = 1;	//	avoiding errors due to comments
		
		result += lexeme.Length;
		position += lexeme.Length;
	}

	return std::string::npos;
}

Gear::StringPosition_t Gear::GetLineColumnByPosition(const std::string source, size_t position) {
	size_t length = source.length();
	if (position >= length) return { std::string::npos, std::string::npos };
	
	StringPosition_t result = { 1, 1 };
	for (size_t i = 0; i < position; i++) {
		if (source[i] == '\r') {
			if (source[i] == '\n') i += 1;
			result.Line += 1;
			result.Column = 1;
		}
		else if (source[i] == '\n') {
			result.Line += 1;
			result.Column = 1;
		}
		else result.Column += 1;
	}

	return result;
}

size_t Gear::GetLengthBeforeLineBreak(const std::string source, size_t position) {
	size_t length = source.length();
	if (position >= length) return std::string::npos;

	Lexeme_t lexeme;
	size_t result = 0;
	for (; position < length;) {
		lexeme = GetLexeme(source, position);
		if (lexeme.Type == GrammarType::LINE_BREAK) return result;

		position += lexeme.Length;
		result += lexeme.Length;
	}

	return result;
}

size_t Gear::FindPositionByType(const std::string source, size_t position, GrammarType grammarType) {
	size_t length = source.length();
	if (position >= length) return std::string::npos;

	Lexeme_t lexeme;
	while (position < length) {
		lexeme = GetLexeme(source, position);
		if (lexeme.Type == grammarType) return position;

		position += lexeme.Length;
	}

	return std::string::npos;
}

bool Gear::GetArgumentsFromNestedExpression(const std::string source, size_t position, std::vector<std::vector<Lexeme_t> > &result) {
	size_t length = source.length();
	if (position >= length) return false;

	Lexeme_t lexeme = GetLexeme(source, position);
	GrammarType closingLimiterType;
	if (lexeme.Type == GrammarType::OPENING_PARENTHESIS_OPERATOR) closingLimiterType = GrammarType::CLOSING_PARENTHESIS_OPERATOR;
	else if (lexeme.Type == GrammarType::OPENING_BRACKET_OPERATOR) closingLimiterType = GrammarType::CLOSING_BRACKET_OPERATOR;
	else if (lexeme.Type == GrammarType::OPENING_BRACE_OPERATOR) closingLimiterType = GrammarType::CLOSING_BRACE_OPERATOR;
	else return false;

	size_t nestedExprLen = GetNestedExpressionLength(source, position, lexeme.Type, closingLimiterType, false);
	if (nestedExprLen == std::string::npos) return false;


	position += lexeme.Length;
	size_t closingLimiterPosition = position + nestedExprLen;
	std::vector<Lexeme_t> tmp;

	size_t level = 0;
	GrammarType prevType = GrammarType::SPACE;
	while (position < closingLimiterPosition) {
		lexeme = GetLexeme(source, position);
		switch (lexeme.Type) {
			case GrammarType::OPENING_PARENTHESIS_OPERATOR:
			case GrammarType::OPENING_BRACKET_OPERATOR:
			case GrammarType::OPENING_BRACE_OPERATOR:
				prevType = lexeme.Type;
				level += 1;
				break;
			case GrammarType::CLOSING_PARENTHESIS_OPERATOR:
			case GrammarType::CLOSING_BRACKET_OPERATOR:
			case GrammarType::CLOSING_BRACE_OPERATOR:
				if (!level) return false;
				prevType = lexeme.Type;
				level -= 1;
				break;
			case GrammarType::COMMA_OPERATOR:
				if (prevType == GrammarType::COMMA_OPERATOR) return false;
				prevType = GrammarType::COMMA_OPERATOR;
				result.push_back(tmp);
				tmp.clear();
				break;
			case GrammarType::COMMENT:
				break;
			default:
				if (lexeme.Group != GrammarGroup::REDUNDANT) prevType = lexeme.Type;
				tmp.push_back(lexeme);
				break;
		}

		position += lexeme.Length;
	}

	if (tmp.size()) result.push_back(tmp);

	return true;
}