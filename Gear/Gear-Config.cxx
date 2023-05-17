#include "Gear-Config.hxx"

bool Gear::Config::JsonWrite(const std::string path, const std::string& source, const std::vector<Lexeme>& lexemes) {
	std::ofstream outputFile(path);
	if (!outputFile.is_open()) return false;

	std::string result = "{\n\t\"Elements\": [\n";
	const size_t count = lexemes.size();
	if (count) {
		std::string value = "";
		for (size_t i = 0; i < count; i++) {
			value = source.substr(lexemes[i].Position, lexemes[i].Length);
			if (value == "\t") value = "\\t";
			else if (value == "\v") value = "\\v";
			else if (value == "\f") value = "\\f";
			else if (value == "\r\n") value = "\\r\\n";
			else if (value == "\n") value = "\\n";
			else if (value == "\r") value = "\\r";

			result += "\t\t{ \"GrammarType\": " + std::to_string(lexemes[i].GrammarType)
				+ ", \"Position\": " + std::to_string(lexemes[i].Position)
				+ ", \"Length\": " + std::to_string(lexemes[i].Length)
				+ ", \"Value\": \"" + value + "\" "
				+ " },\n";
		}

		result.erase(result.end() - 2);
	}

	result += "\t]\n}";
	outputFile << result;
	outputFile.close();
	return true;
}