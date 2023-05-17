#include "Gear-Lexer.hxx"

Gear::Token::operator std::string() const {
	std::string representation = "GrammarType: " + std::to_string(GrammarType) + "; Pattern: " + Pattern;
	return representation;
}

Gear::Lexeme::operator std::string() const {
	std::string representation = "GrammarType: " + std::to_string(GrammarType) + "; Pos: " + std::to_string(Position) + "; Len: " + std::to_string(Length);
	return representation;
}

Gear::Lexeme Gear::Lexer::GetLexeme(const std::string& source, size_t position, const std::vector<Token>& tokens) {
	const size_t length = source.length();
	if (position >= length) return { GRAMMAR_TYPE_EOF, position, 0 };

	std::regex tokenExpression;
	std::smatch tokenMatch;
	std::string subString = source.substr(position);
	
	const size_t count = tokens.size();
	for (size_t i = 0; i < count; i++) {
		tokenExpression = std::regex(tokens[i].Pattern);
		if (std::regex_search(subString, tokenMatch, tokenExpression) && !tokenMatch.position()) return { tokens[i].GrammarType, position, tokenMatch.str().length() };
	}

	return { GRAMMAR_TYPE_UNDEFINED, position, 1 };
}

size_t Gear::Lexer::FindNextPosition(const std::string& source, size_t position, const std::vector<Token>& tokens) {
	const size_t length = source.length();
	if (position >= length) return std::string::npos;

	Lexeme lexeme = GetLexeme(source, position, tokens);
	if (lexeme.GrammarType == GRAMMAR_TYPE_UNDEFINED || lexeme.GrammarType == GRAMMAR_TYPE_EOF) return std::string::npos;
	position += lexeme.Length;

	do {
		lexeme = GetLexeme(source, position, tokens);
		if (lexeme.GrammarType == GRAMMAR_TYPE_EOF) break;
		else if (lexeme.GrammarType != GRAMMAR_TYPE_SPACE && lexeme.GrammarType != GRAMMAR_TYPE_LINE_BREAK) return position;

		position += lexeme.Length;
	} while (position < length);

	return std::string::npos;
}

size_t Gear::Lexer::FindPreviousPosition(const std::string& source, size_t position, const std::vector<Token>& tokens) {
	const size_t length = source.length();
	if (position >= length) return std::string::npos;

	Lexeme lexeme = GetLexeme(source, position, tokens);
	if (lexeme.GrammarType == GRAMMAR_TYPE_UNDEFINED || lexeme.GrammarType == GRAMMAR_TYPE_EOF) return std::string::npos;
	position -= lexeme.Length;

	do {
		lexeme = GetLexeme(source, position, tokens);
		if (lexeme.GrammarType == GRAMMAR_TYPE_EOF) break;
		else if (lexeme.GrammarType != GRAMMAR_TYPE_SPACE && lexeme.GrammarType != GRAMMAR_TYPE_LINE_BREAK) return position;

		position -= lexeme.Length;
	} while (position < length);

	return std::string::npos;
}

std::vector<Gear::Lexeme> Gear::Lexer::Lex(const std::string& source, size_t position, const std::vector<Token>& tokens) {
	Lexeme lexeme;
	std::vector<Lexeme> lexemes;
	while (true) {
		lexeme = GetLexeme(source, position, tokens);
		if (lexeme.GrammarType == GRAMMAR_TYPE_EOF) break;
		lexemes.push_back(lexeme);
		position += lexeme.Length;
	}
	return lexemes;
}