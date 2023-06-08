#include <main.hpp>

void PrintInfo() {
	size_t majorVer = (GEAR_VERSION >> 16) & 0xff;
	size_t minorVer = (GEAR_VERSION >> 8) & 0xff;
	size_t buildVer = GEAR_VERSION & 0xff;
	std::cout << "Gear programming language toolkit by github.com/TyfoC" << std::endl;
	std::cout << "Version: " << majorVer << '.' << minorVer << '.' << buildVer << std::endl;
}

void PrintHelp() {
	std::cout << "Usage: gear32/64 <-action> [<-options>] [<-input file paths>]" << std::endl;
	std::cout << "Actions:" << std::endl;
	std::cout << "\t-info - show version info" << std::endl;
	std::cout << "\t-help - show help" << std::endl;
	std::cout << "\t-list-tokens - list tokens (-o, -f)" << std::endl;
	std::cout << "\t-lex - generate lexeme table from file (-o, -f)" << std::endl;
	std::cout << "\t-preprocess - preprocess file (-o, -I)" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "\t-o - select output path" << std::endl;
	std::cout << "\t-f - select output format" << std::endl;
	std::cout << "\t\tOutput formats: std, json" << std::endl;
	std::cout << "\t-I - include directory" << std::endl;
}

int main(int argc, char** argv) {
	Action action = Action::UNDEFINED;
	std::string outputPath = "";
	OutputFormat format = OutputFormat::STANDARD;
	std::vector<std::string> includePaths;
	std::vector<std::string> inputPaths;

	for (size_t i = 1; i < (size_t)argc; i++) {
		if (!strcmp(argv[i], "-info")) action = Action::INFO;
		else if (!strcmp(argv[i], "-help")) action = Action::HELP;
		else if (!strcmp(argv[i], "-list-tokens")) action = Action::LIST_TOKENS;
		else if (!strcmp(argv[i], "-lex")) action = Action::LEX;
		else if (!strcmp(argv[i], "-preprocess")) action = Action::PREPROCESS;
		else if (!strcmp(argv[i], "-o")) {
			if ((i += 1) >= (size_t)argc) std::cout << "Warning: missing output path!" << std::endl;
			if (outputPath[0]) {
				std::cout << "Warning: The result path was already taken, now it has changed (`" << outputPath << "`->`";
				std::cout << argv[i] << "`)!" << std::endl;
			}

			outputPath = std::string(argv[i]);
		}
		else if (!strcmp(argv[i], "-f")) {
			if ((i += 1) >= (size_t)argc) std::cout << "Warning: missing output format!" << std::endl;
			else {
				if (!strcmp(argv[i], "std")) format = OutputFormat::STANDARD;
				else if (!strcmp(argv[i], "json")) format = OutputFormat::JSON;
				else std::cout << "Warning: this format is not supported (`" << argv[i] << "`)!" << std::endl;
			}
		}
		else if (!strcmp(argv[i], "-I")) {
			if ((i += 1) >= (size_t)argc) std::cout << "Warning: missing include directory path!" << std::endl;
			else includePaths.push_back(std::string(argv[i]));
		}
		else inputPaths.push_back(std::string(argv[i]));
	}

	if (action == Action::UNDEFINED) std::cout << "Warning: no action selected, write `-help` to list actions!" << std::endl;
	else if (action == Action::INFO) PrintInfo();
	else if (action == Action::HELP) PrintHelp();
	else if (action == Action::LIST_TOKENS) {
		std::string result = "";
		if (format == OutputFormat::STANDARD) {
			for (const Gear::Token_t &token : Gear::GetGrammar()) {
				result += "Pattern: `" + token.Pattern +
					"`, Group: " + std::to_string((size_t)token.Group) + 
					", Type: " + std::to_string((size_t)token.Type) +
				",\n";
			}

			result.erase(result.length() - 2, 1);
		}
		else if (format == OutputFormat::JSON) {
			result += "{\n\t\"tokens\": [\n";

			Gear::JsonElement_t element;
			for (const Gear::Token_t &token : Gear::GetGrammar()) {
				element = Gear::TokenToJsonElement(token);
				result += Gear::StringifyJsonElement(element, false, 2) + ",\n";
			}

			result.erase(result.length() - 2, 1);
			result += "\t]\n}";
		}

		if (outputPath[0]) {
			std::ofstream output(outputPath);
			if (!output.is_open()) {
				std::cout << "Error: failed to open file `" << outputPath << "`!" << std::endl;
				return 1;
			}

			output << result;
			output.close();
		}
		else std::cout << result << std::endl;
	}
	else if (action == Action::LEX) {
		if (inputPaths.size() != 1) {
			std::cout << "Error: `-lex` action can only be applied to one file!" << std::endl;
			return 1;
		}

		std::ifstream input(inputPaths[0]);
		if (!input.is_open()) {
			std::cout << "Error: failed to open file `" << inputPaths[0] << "`!" << std::endl;
			return 1;
		}

		std::string fileData = Gear::ReadTextFile(input);
		input.close();

		std::string result = "";
		std::vector<Gear::Lexeme_t> lexemes = Gear::Lex(fileData);
		if (format == OutputFormat::STANDARD) {
			for (const Gear::Lexeme_t &lexeme : lexemes) {
				result += '`' + Gear::FormatString(fileData.substr(lexeme.Position, lexeme.Length)) + "`: "
					"group: " + std::to_string((size_t)lexeme.Group) +
					", type: " + std::to_string((size_t)lexeme.Type) +
					", position: " + std::to_string(lexeme.Position) +
					", length: " + std::to_string(lexeme.Length) +
				",\n";
			}

			result.erase(result.length() - 2, 1);
		}
		else if (format == OutputFormat::JSON) {
			result += "{\n\t\"lexemes\": [\n";

			Gear::JsonElement_t element;
			for (const Gear::Lexeme_t &lexeme : lexemes) {
				element = Gear::LexemeToJsonElement(fileData, lexeme);
				result += Gear::StringifyJsonElement(element, false, 2) + ",\n";
			}

			result.erase(result.length() - 2, 1);
			result += "\t]\n}";
		}

		if (outputPath[0]) {
			std::ofstream output(outputPath);
			if (!output.is_open()) {
				std::cout << "Error: failed to open file `" << outputPath << "`!" << std::endl;
				return 1;
			}

			output << result;
			output.close();
		}
		else std::cout << result << std::endl;
	}
	else if (action == Action::PREPROCESS) {
		if (inputPaths.size() != 1) {
			std::cout << "Error: `-preprocess` action can only be applied to one file!" << std::endl;
			return 1;
		}

		std::ifstream input(inputPaths[0]);
		if (!input.is_open()) {
			std::cout << "Error: failed to open file `" << inputPaths[0] << "`!" << std::endl;
			return 1;
		}

		std::string fileData = Gear::ReadTextFile(input);
		input.close();

		std::vector<Gear::Macro_t> macros;
		Gear::PreprocessingResult_t result = Gear::Preprocess(
			fileData,
			inputPaths[0],
			includePaths,
			macros
		);
		
		Gear::PrintMessages(result.Messages, inputPaths[0], fileData);

		if (!result.WorkCompleted) {
			std::cout << "Error: failed to preprocess file `" << inputPaths[0] << "`!" << std::endl;
			return 1;
		}

		if (outputPath[0]) {
			std::ofstream output(outputPath);
			if (!output.is_open()) {
				std::cout << "Error: failed to open file `" << outputPath << "`!" << std::endl;
				return 1;
			}

			output << result.Output;
			output.close();
		}
		else std::cout << result.Output << std::endl;
	}
}