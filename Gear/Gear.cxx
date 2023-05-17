#include "Gear.hxx"

void PrintInfo() {
	std::cout << "Gear Programming Language v" << GEAR_VERSION << std::endl;
	std::cout << "Author's Github: github.com/TyfoC" << std::endl;
	std::cout << "\"Gear -help\" to get help" << std::endl << std::endl;
}

void PrintHelp() {
	std::cout << "Gear -action inputPath1 inputPathN -option -param value" << std::endl;
	std::cout << "Actions:" << std::endl;
	std::cout << "\t-info - show info about program" << std::endl;
	std::cout << "\t-help - show help" << std::endl;
	std::cout << "\t-blt - Build Lexeme Table" << std::endl;
	std::cout << "Options: N/A" << std::endl;
	std::cout << "Params:" << std::endl;
	std::cout << "\t-i includeDirectoryPath - specify Include directory" << std::endl;
	std::cout << "\t-o outputPath - specify Output file path" << std::endl;
}

int main(int argc, char** argv) {
	Gear::Action action = Gear::Action::UNDEFINED;
	std::vector<std::string> options;
	std::vector<std::string> inputPaths;
	std::vector<std::string> includeDirectories;
	std::string outputPath = "";

	size_t count = (size_t)argc;
	for (size_t i = 1; i < count; i++) {
		if (!strcmp(argv[i], "-info")) PrintInfo();
		else if (!strcmp(argv[i], "-help")) PrintHelp();
		else if (!strcmp(argv[i], "-blt")) action = Gear::Action::BUILD_LEXEME_TABLE;
		else if (!strcmp(argv[i], "-i")) {
			if (++i >= count) {
				std::cerr << "Error: missing include directory path!" << std::endl;
				return Gear::RETURN_CODE_MISSING_INCLUDE_DIRECTORY_PATH;
			}

			includeDirectories.push_back(argv[i]);
		}
		else if (!strcmp(argv[i], "-o")) {
			if (outputPath[0]) {
				std::cerr << "Error: output file path already selected!" << std::endl;
				return Gear::RETURN_CODE_OUTPUT_FILE_PATH_ALREADY_SELECTED;
			}

			if (++i >= count) {
				std::cerr << "Error: missing output file path!" << std::endl;
				return Gear::RETURN_CODE_MISSING_INCLUDE_DIRECTORY_PATH;
			}

			outputPath = argv[i];
		}
		else inputPaths.push_back(argv[i]);
	}

	if (action == Gear::Action::UNDEFINED) {
		std::cerr << "Error: no action selected!" << std::endl;
		return Gear::RETURN_CODE_NO_ACTION_SELECTED;
	}
	else if (action == Gear::Action::BUILD_LEXEME_TABLE) {
		if (!outputPath[0]) outputPath = Gear::DefaultOutputFileName + ".json";
		for (const std::string& inputPath : inputPaths) {
			Gear::FileStream fileStream = Gear::FileStream(inputPath);
			if (!fileStream.isOpen()) {
				std::cerr << "Error: failed to open file \"" << inputPath << "\"!" << std::endl;
				return Gear::RETURN_CODE_FAILED_TO_OPEN_FILE;
			}

			std::string fileText = fileStream.readText();
			std::vector<Gear::Lexeme> lexemes = Gear::Lexer::Lex(fileText, 0, Gear::Tokens);
			if (!Gear::Config::JsonWrite(outputPath, fileText, lexemes)) {
				fileStream.close();
				std::cerr << "Error: cannot write file \"" << outputPath << "\"!" << std::endl;
				return Gear::RETURN_CODE_FAILED_WRITE_FILE;
			}

			fileStream.close();
		}
	}

	return Gear::RETURN_CODE_SUCCESS;
}