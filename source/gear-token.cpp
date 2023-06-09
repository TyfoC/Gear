#include <gear-token.hpp>

Gear::Token::operator std::string() const {
	return "group: " + std::to_string(Group) + ", type: " + std::to_string(Type) +
		", position: " + std::to_string(Position) + ", lexeme: `" + Lexeme + '`';
}

size_t Gear::GetTokenIndex(const std::vector<Token> &tokens, const std::string &value) {
	size_t count = tokens.size();
	for (size_t i = 0; i < count; i++) if (tokens[i].Lexeme == value) return i;
	return std::string::npos;
}