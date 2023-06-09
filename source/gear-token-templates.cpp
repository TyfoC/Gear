#include <gear-token-templates.hpp>
static std::vector<Gear::TokenTemplate> TokenTemplates = {
	{ Gear::Token::GROUP_REDUNDANT, Gear::Token::TYPE_SPACE, R"([ \t\v\f])" },
	{ Gear::Token::GROUP_REDUNDANT, Gear::Token::TYPE_LINE_BREAK, R"((\r\n|\r|\n))" },
	{ Gear::Token::GROUP_REDUNDANT, Gear::Token::TYPE_COMMENT, R"((\/\*([\s\S]*?)\*\/|\/\/.*))" },
	{ Gear::Token::GROUP_LITERAL, Gear::Token::TYPE_HEX_LITERAL, R"(0[xX][\da-fA-F']+)" },
	{ Gear::Token::GROUP_LITERAL, Gear::Token::TYPE_DEC_LITERAL, R"(\d+\b)" },
	{ Gear::Token::GROUP_LITERAL, Gear::Token::TYPE_OCT_LITERAL, R"(0[oO]?[0-7']+\b)" },
	{ Gear::Token::GROUP_LITERAL, Gear::Token::TYPE_BIN_LITERAL, R"(0[bB][01]+\b)" },
	{ Gear::Token::GROUP_LITERAL, Gear::Token::TYPE_CHAR_LITERAL, R"('(\\[\s\S]|[^\'])*')" },
	{ Gear::Token::GROUP_LITERAL, Gear::Token::TYPE_STRING_LITERAL, R"("(\\[\s\S]|[^\"])*")" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_ELLIPSIS_OPERATOR, R"(\.\.\.)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_INC_OPERATOR, R"(\+\+)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_DEC_OPERATOR, R"(\-\-)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_LSH_ASSIGN_OPERATOR, R"(<<=)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_RSH_ASSIGN_OPERATOR, R"(>>=)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_EQUAL_TO_OPERATOR, R"(==)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_NOT_EQUAL_OPERATOR, R"(!=)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_GREATER_OR_EQUAL_OPERATOR, R"(>=)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_LESS_OR_EQUAL_OPERATOR, R"(<=)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_LSH_OPERATOR, R"(<<)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_RSH_OPERATOR, R"(>>)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_AND_ASSIGN_OPERATOR, R"(&=)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_OR_ASSIGN_OPERATOR, R"(\|=)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_XOR_ASSIGN_OPERATOR, R"(\^=)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_ADD_OPERATOR, R"(\+)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_SUB_OPERATOR, R"(\-)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_MUL_OPERATOR, R"(\*)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_DIV_OPERATOR, R"(\/)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_MOD_OPERATOR, R"(%)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_GREATER_THAN_OPERATOR, R"(>)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_LESS_THAN_OPERATOR, R"(<)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_NOT_OPERATOR, R"(!)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_AND_OPERATOR, R"(&)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_OR_OPERATOR, R"(\|)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_XOR_OPERATOR, R"(\^)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_COMPLEMENT_OPERATOR, R"(~)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_ASSIGN_OPERATOR, R"(=)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_OPENING_PARENTHESIS_OPERATOR, R"(\()" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_CLOSING_PARENTHESIS_OPERATOR, R"(\))" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_OPENING_BRACKET_OPERATOR, R"(\[)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_CLOSING_BRACKET_OPERATOR, R"(\])" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_OPENING_BRACE_OPERATOR, R"(\{)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_CLOSING_BRACE_OPERATOR, R"(\})" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_DOT_OPERATOR, R"(\.)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_COMMA_OPERATOR, R"(,)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_COLON_OPERATOR, R"(\:)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_SEMICOLON_OPERATOR, R"(;)" },
	{ Gear::Token::GROUP_OPERATOR, Gear::Token::TYPE_PREPROCESSOR_OPERATOR, R"(@)" },
	{ Gear::Token::GROUP_IDENTIFIER, Gear::Token::TYPE_IDENTIFIER, R"([_a-zA-Z]\w*\b)" }
};

std::vector<Gear::TokenTemplate> &Gear::GetTokenTemplates() {
	return TokenTemplates;
}

size_t Gear::GetTokenTemplateIndex(size_t typeIndex) {
	size_t count = TokenTemplates.size();
	for (size_t i = 0; i < count; i++) if (TokenTemplates[i].Type == typeIndex) return i;
	return std::string::npos;
}