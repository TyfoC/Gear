#include "Gear-Utils.hxx"

size_t Gear::GetSymbolsCountBeforeLineBreak(const std::string& source, size_t position) {
	const size_t length = source.length();
	if (position >= length) return 0;

	size_t lineBreakPosition = source.find('\r', position);
	if (lineBreakPosition == std::string::npos) {
		lineBreakPosition = source.find('\n', position);
		if (lineBreakPosition == std::string::npos) return source.substr(position).length();
	}

	return lineBreakPosition - position - 1;
}