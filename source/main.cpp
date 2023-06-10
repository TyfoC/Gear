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
	std::cout << "\t-tokenize - generate token table from file (-o, -f)" << std::endl;
	std::cout << "\t-preprocess - preprocess file (-o, -I)" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "\t-o - select output path" << std::endl;
	std::cout << "\t-f - select output format" << std::endl;
	std::cout << "\t\tOutput formats: std, json" << std::endl;
	std::cout << "\t-I - include directory" << std::endl;
}

int main(int argc, char** argv) {
	size_t action = ACTION_UNDEFINED;
	size_t format = FORMAT_STANDARD;
	char* outputPath = 0;
	std::vector<std::string> inputPaths;
	std::vector<std::string> includePaths;

	size_t count = (size_t)argc;
	for (size_t i = 1; i < count; i++) {
		if (!strcmp(argv[i], "-info")) {
			if (action != ACTION_UNDEFINED) std::cout << "Warning: action changed!" << std::endl;
			action = ACTION_INFO;
		}
		else if (!strcmp(argv[i], "-help")) {
			if (action != ACTION_UNDEFINED) std::cout << "Warning: action changed!" << std::endl;
			action = ACTION_HELP;
		}
		else if (!strcmp(argv[i], "-tokenize")) {
			if (action != ACTION_UNDEFINED) std::cout << "Warning: action changed!" << std::endl;
			action = ACTION_TOKENIZE;
		}
		else if (!strcmp(argv[i], "-preprocess")) {
			if (action != ACTION_UNDEFINED) std::cout << "Warning: action changed!" << std::endl;
			action = ACTION_PREPROCESS;
		}
		else if (!strcmp(argv[i], "-o")) {
			if (i >= count - 1) std::cout << "Warning: missing output path!" << std::endl;
			else outputPath = argv[++i];
		}
		else if (!strcmp(argv[i], "-f")) {
			if (i >= count - 1) std::cout << "Warning: missing format!" << std::endl;
			else {
				i += 1;
				if (!strcmp(argv[i], "std")) format = FORMAT_STANDARD;
				else if (!strcmp(argv[i], "json")) format = FORMAT_JSON;
				else std::cout << "Warning: unknown format `" << argv[i] << "`!" << std::endl;
			}
		}
		else if (!strcmp(argv[i], "-I")) {
			if (i >= count - 1) std::cout << "Warning: missing directory path!" << std::endl;
			else includePaths.push_back(std::string(argv[++i]));
		}
		else inputPaths.push_back(std::string(argv[i]));
	}

	if (action == ACTION_UNDEFINED) std::cout << "Warning: action not selected!" << std::endl;
	else if (action == ACTION_INFO) PrintInfo();
	else if (action == ACTION_HELP) PrintHelp();
	else if (action == ACTION_TOKENIZE) {
		if (inputPaths.size() != 1) {
			std::cout << "Error: `-tokenize` action requires only one file path!" << std::endl;
			return 1;
		}

		std::ifstream input(inputPaths[0]);
		if (!input.is_open()) {
			std::cout << "Error: failed to open `" << inputPaths[0] << "`!" << std::endl;
			return 2;
		}

		std::string fileData = Gear::ReadTextFile(input);
		input.close();

		Gear::Scanner scanner(fileData);
		std::vector<Gear::Token> tokens = scanner.ScanToEnd();
		size_t tokensCount = tokens.size();

		std::string result = "";
		if (format == FORMAT_STANDARD) {
			for (size_t i = 0; i < tokensCount; i++) {
				std::cout << i + 1 << ") " << (std::string)tokens[i] << std::endl;
			}
		}
		else if (format == FORMAT_JSON) {
			result += "{\n\t\"tokens\": [\n";
			for (const Gear::Token &token : tokens) result += Gear::StringifyJsonElement(Gear::TokenToJsonElement(token), false, 2) + ",\n";
			result.erase(result.length() - 2, 1);
			result += "\t]\n}";
		}
		else {
			std::cout << "Error: unknown output format!" << std::endl;
			return 3;
		}

		if (outputPath) {
			std::ofstream output(outputPath);
			if (!output.is_open()) {
				std::cout << "Error: failed to write `" << outputPath << "`!" << std::endl;
				return 4;
			}

			output << result;
			output.close();
		}
		else std::cout << result << std::endl;
	}
	else if (action == ACTION_PREPROCESS) {
		if (inputPaths.size() != 1) {
			std::cout << "Error: `-tokenize` action requires only one file path!" << std::endl;
			return 1;
		}

		std::ifstream input(inputPaths[0]);
		if (!input.is_open()) {
			std::cout << "Error: failed to open `" << inputPaths[0] << "`!" << std::endl;
			return 2;
		}

		std::string fileData = Gear::ReadTextFile(input);
		input.close();

		std::vector<Gear::Macro> macros;
		Gear::PreprocessingResult result = Gear::Preprocessor::Preprocess(fileData, inputPaths[0], includePaths, macros);
		Gear::PrintMessages(result.Messages, inputPaths[0], fileData);
		if (!result.IsWorkCompleted) return 5;

		if (format != FORMAT_STANDARD) {
			std::cout << "Error: only `std` format is available for `-preprocess` action!" << std::endl;
			return 6;
		}

		if (outputPath) {
			std::ofstream output(outputPath);
			if (!output.is_open()) {
				std::cout << "Error: failed to write `" << outputPath << "`!" << std::endl;
				return 4;
			}

			output << result.Output;
			output.close();
		}
		else std::cout << result.Output << std::endl;
	}
}