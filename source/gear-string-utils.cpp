#include <gear-string-utils.hpp>

Gear::StringPosition Gear::GetStringPosition(const std::string &source, size_t position) {
	StringPosition strPos = { 1, 1 };
	Scanner scanner(source);
	Token token;
	while (scanner.GetPosition() != position) {
		token = scanner.ScanNext();
		if (token.Type == Token::TYPE_LINE_BREAK) {
			strPos.Line += 1;
			strPos.Column = 1;
		}
		else strPos.Column += token.Lexeme.length();
	}

	return strPos;
}

size_t Gear::GetCharactersCountBeforeLineBreak(const std::string &source, size_t position) {
	size_t length = source.length();
	if (position >= length) return std::string::npos;

	Scanner scanner(source);
	scanner.SetPosition(position);

	Token token;
	size_t result = 0;
	while (position < length) {
		token = scanner.ScanNext();
		if (token.Type == Token::TYPE_LINE_BREAK) return result;
		result += token.Lexeme.length();
	}

	return result;
}