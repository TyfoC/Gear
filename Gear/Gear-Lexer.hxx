#pragma once
#ifndef GEAR_LEXER
#define GEAR_LEXER

#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include "Gear-Message.hxx"
#include "Gear-Result.hxx"

namespace Gear {
	enum BasicGrammarType {
		GRAMMAR_TYPE_UNDEFINED,
		GRAMMAR_TYPE_EOF,
		GRAMMAR_TYPE_SPACE,
		GRAMMAR_TYPE_LINE_BREAK
	};

	struct Token {
		operator std::string() const;
		size_t		GrammarType;		//	0-3 reserved
		std::string	Pattern;
	};

	struct Lexeme {
		operator std::string() const;
		size_t	GrammarType;			//	0-3 reserved
		size_t	Position;
		size_t	Length;
	};

	using LexResult = ExecutionResult<std::vector<Lexeme> >;

	class Lexer {
	public:
		static Lexeme GetLexeme(const std::string& source, size_t position, const std::vector<Token>& tokens);
		static size_t FindNextPosition(const std::string& source, size_t position, const std::vector<Token>& tokens);
		static size_t FindPreviousPosition(const std::string& source, size_t position, const std::vector<Token>& tokens);
		static LexResult Lex(const std::string& source, size_t position, const std::vector<Token>& tokens);
	};
}

#endif