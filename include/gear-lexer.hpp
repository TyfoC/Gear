#pragma once

#include <string>
#include <vector>
#include <regex>
#include "gear-typedefs.hpp"

namespace Gear
{
	enum class GrammarGroup {
		UNDEFINED,
		REDUNDANT,
		LITERAL,
		OPERATOR,
		IDENTIFIER
	};

	enum class GrammarType {
		UNDEFINED,
		SPACE,
		LINE_BREAK,
		COMMENT,
		HEX_LITERAL,
		DEC_LITERAL,
		OCT_LITERAL,
		BIN_LITERAL,
		CHAR_LITERAL,
		STRING_LITERAL,
		ELLIPSIS_OPERATOR,
		ADD_OPERATOR,
		SUB_OPERATOR,
		MUL_OPERATOR,
		DIV_OPERATOR,
		MOD_OPERATOR,
		INC_OPERATOR,
		DEC_OPERATOR,
		EQUAL_TO_OPERATOR,
		NOT_EQUAL_OPERATOR,
		GREATER_THAN_OPERATOR,
		LESS_THAN_OPERATOR,
		GREATER_OR_EQUAL_OPERATOR,
		LESS_OR_EQUAL_OPERATOR,
		NOT_OPERATOR,
		AND_OPERATOR,
		OR_OPERATOR,
		XOR_OPERATOR,
		COMPLEMENT_OPERATOR,
		LSH_OPERATOR,
		RSH_OPERATOR,
		ASSIGN_OPERATOR,
		ADD_ASSIGN_OPERATOR,
		SUB_ASSIGN_OPERATOR,
		MUL_ASSIGN_OPERATOR,
		DIV_ASSIGN_OPERATOR,
		MOD_ASSIGN_OPERATOR,
		LSH_ASSIGN_OPERATOR,
		RSH_ASSIGN_OPERATOR,
		AND_ASSIGN_OPERATOR,
		OR_ASSIGN_OPERATOR,
		XOR_ASSIGN_OPERATOR,
		OPENING_PARENTHESIS_OPERATOR,
		CLOSING_PARENTHESIS_OPERATOR,
		OPENING_BRACKET_OPERATOR,
		CLOSING_BRACKET_OPERATOR,
		OPENING_BRACE_OPERATOR,
		CLOSING_BRACE_OPERATOR,
		DOT_OPERATOR,
		COMMA_OPERATOR,
		COLON_OPERATOR,
		SEMICOLON_OPERATOR,
		PREPROCESSOR_OPERATOR,
		IDENTIFIER
	};
	
	struct Token_t
	{
		GrammarGroup	Group;
		GrammarType		Type;
		std::string		Pattern;
	};

	struct Lexeme_t
	{
		GrammarGroup	Group;
		GrammarType		Type;
		size_t			Position;
		size_t			Length;
		std::string		Value;
	};

	struct StringPosition_t {
		size_t	Line;
		size_t	Column;	
	};

	std::vector<Lexeme_t> Lex(const std::string source);
	Lexeme_t GetLexeme(const std::string source, size_t position);
	std::vector<Token_t> &GetGrammar();
	size_t FindTokenIndex(GrammarType grammarType);

	size_t GetNestingLength(
		const std::string source,
		size_t position,
		GrammarType leftLimiterType,
		GrammarType rightLimiterType,
		bool avoidComments = true
	);

	size_t GetNestedExpressionLength(
		const std::string source,
		size_t position,
		GrammarType leftLimiterType,
		GrammarType rightLimiterType,
		bool avoidComments = true
	);

	StringPosition_t GetLineColumnByPosition(const std::string source, size_t position);
	size_t GetLengthBeforeLineBreak(const std::string source, size_t position);
	size_t FindPositionByType(const std::string source, size_t position, GrammarType grammarType);
	bool GetArgumentsFromNestedExpression(const std::string source, size_t position, std::vector<std::vector<Lexeme_t> > &result);
	bool GetArgumentsFromNestedExpression(const std::vector<Lexeme_t> lexemes, size_t index, std::vector<std::vector<Lexeme_t> > &result);
	size_t GetLexemeIndex(const std::vector<Lexeme_t> lexemes, const std::string value);
}