#pragma once

#include <string>
#include <vector>

namespace Gear {
	struct Token {
		enum {
			GROUP_UNDEFINED,
			GROUP_REDUNDANT,
			GROUP_LITERAL,
			GROUP_OPERATOR,
			GROUP_IDENTIFIER,
			GROUP_EOF
		};

		enum {
			TYPE_UNDEFINED,
			TYPE_SPACE,
			TYPE_LINE_BREAK,
			TYPE_COMMENT,
			TYPE_HEX_LITERAL,
			TYPE_DEC_LITERAL,
			TYPE_OCT_LITERAL,
			TYPE_BIN_LITERAL,
			TYPE_CHAR_LITERAL,
			TYPE_STRING_LITERAL,
			TYPE_ELLIPSIS_OPERATOR,
			TYPE_ADD_OPERATOR,
			TYPE_SUB_OPERATOR,
			TYPE_MUL_OPERATOR,
			TYPE_DIV_OPERATOR,
			TYPE_MOD_OPERATOR,
			TYPE_INC_OPERATOR,
			TYPE_DEC_OPERATOR,
			TYPE_EQUAL_TO_OPERATOR,
			TYPE_NOT_EQUAL_OPERATOR,
			TYPE_GREATER_THAN_OPERATOR,
			TYPE_LESS_THAN_OPERATOR,
			TYPE_GREATER_OR_EQUAL_OPERATOR,
			TYPE_LESS_OR_EQUAL_OPERATOR,
			TYPE_NOT_OPERATOR,
			TYPE_AND_OPERATOR,
			TYPE_OR_OPERATOR,
			TYPE_XOR_OPERATOR,
			TYPE_COMPLEMENT_OPERATOR,
			TYPE_LSH_OPERATOR,
			TYPE_RSH_OPERATOR,
			TYPE_ASSIGN_OPERATOR,
			TYPE_ADD_ASSIGN_OPERATOR,
			TYPE_SUB_ASSIGN_OPERATOR,
			TYPE_MUL_ASSIGN_OPERATOR,
			TYPE_DIV_ASSIGN_OPERATOR,
			TYPE_MOD_ASSIGN_OPERATOR,
			TYPE_LSH_ASSIGN_OPERATOR,
			TYPE_RSH_ASSIGN_OPERATOR,
			TYPE_AND_ASSIGN_OPERATOR,
			TYPE_OR_ASSIGN_OPERATOR,
			TYPE_XOR_ASSIGN_OPERATOR,
			TYPE_OPENING_PARENTHESIS_OPERATOR,
			TYPE_CLOSING_PARENTHESIS_OPERATOR,
			TYPE_OPENING_BRACKET_OPERATOR,
			TYPE_CLOSING_BRACKET_OPERATOR,
			TYPE_OPENING_BRACE_OPERATOR,
			TYPE_CLOSING_BRACE_OPERATOR,
			TYPE_DOT_OPERATOR,
			TYPE_COMMA_OPERATOR,
			TYPE_COLON_OPERATOR,
			TYPE_SEMICOLON_OPERATOR,
			TYPE_PREPROCESSOR_OPERATOR,
			TYPE_IDENTIFIER,
			TYPE_EOF
		};

		operator std::string() const;
		size_t		Group;
		size_t		Type;
		size_t		Position;
		std::string	Lexeme;
		void		*Literal;
	};

	size_t GetTokenIndex(const std::vector<Token> &tokens, const std::string &value);
}