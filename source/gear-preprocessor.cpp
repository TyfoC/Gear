#include <gear-preprocessor.hpp>

Gear::PreprocessingResult_t Gear::Preprocess(
	const std::string source,
	const std::string srcFileDirectoryPath,
	const std::vector<std::string> includePaths
) {
	PreprocessingResult_t result = { false, "", {} };

	Lexeme_t lexeme;
	StringPosition_t strPos;
	size_t length = source.length();
	for (size_t i = 0; i < length;) {
		lexeme = GetLexeme(source, i);

		if (lexeme.Group == GrammarGroup::UNDEFINED) {
			strPos = GetLineColumnByPosition(source, i);
			result.Messages.push_back({ MessageType::ERROR, "unknown lexeme found", strPos.Line, strPos.Column, i });
			return result;
		}
		else if (lexeme.Type == GrammarType::PREPROCESSOR_OPERATOR) {
			std::string_view preprocessorOperator = std::string_view(source).substr(i, lexeme.Length);
			if ((i += lexeme.Length) >= length) {
				strPos = GetLineColumnByPosition(source, i);
				result.Messages.push_back({ MessageType::ERROR, "missing preprocessor directive name", strPos.Line, strPos.Column, i });
				return result;
			}

			do {
				lexeme = GetLexeme(source, i);
				if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
			} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);

			if (lexeme.Group != GrammarGroup::IDENTIFIER) {
				strPos = GetLineColumnByPosition(source, i);
				result.Messages.push_back({ MessageType::ERROR, "missing preprocessor directive name", strPos.Line, strPos.Column, i });
				return result;
			}

			std::string_view directiveName = std::string_view(source).substr(i, lexeme.Length);
			if (directiveName == "op") {
				result.Output += preprocessorOperator;
				if ((i += lexeme.Length) >= length) break;

				lexeme = GetLexeme(source, i);
				if (lexeme.Type == GrammarType::PREPROCESSOR_OPERATOR) {
					if ((i += lexeme.Length) >= length) break;
					do {
						lexeme = GetLexeme(source, i);
						if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
					} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);
				}
			}
			else if (directiveName == "join") {
				if ((i += lexeme.Length) >= length) break;
				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);
			}
			else if (directiveName == "save") {
				if ((i += lexeme.Length) >= length) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing expression", strPos.Line, strPos.Column, i });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);

				if (lexeme.Type != GrammarType::OPENING_BRACE_OPERATOR) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing expression", strPos.Line, strPos.Column, i });
					return result;
				}

				size_t nestedExprLength = GetNestedExpressionLength(
					source, i, GrammarType::OPENING_BRACE_OPERATOR, GrammarType::CLOSING_BRACE_OPERATOR
				);

				if (nestedExprLength == std::string::npos) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing end of expression", strPos.Line, strPos.Column, i });
					return result;
				}

				i += lexeme.Length;

				result.Output += source.substr(i, nestedExprLength);
				i += nestedExprLength;

				lexeme = GetLexeme(source, i);
				i += lexeme.Length;
			}
			else if (directiveName == "preinc" || directiveName == "rawinc") {
				bool preprocessFile = directiveName[0] == 'p';

				if ((i += lexeme.Length) >= length) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing file path", strPos.Line, strPos.Column, i });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);

				std::string filePath = "";
				std::ifstream handle;
				if (lexeme.Type == GrammarType::STRING_LITERAL) {
					filePath = srcFileDirectoryPath + '/' + source.substr(lexeme.Position + 1, lexeme.Length - 2);
					handle = std::ifstream(filePath);
					if (!handle.is_open()) {
						strPos = GetLineColumnByPosition(source, i);
						result.Messages.push_back({ MessageType::ERROR, "failed to open `" + filePath + '`', strPos.Line, strPos.Column, i });
						return result;
					}

					i += lexeme.Length;
				}
				else if (lexeme.Type == GrammarType::LESS_THAN_OPERATOR) {
					if ((i += lexeme.Length) >= length) {
						strPos = GetLineColumnByPosition(source, i);
						result.Messages.push_back({ MessageType::ERROR, "missing file path", strPos.Line, strPos.Column, i });
						return result;
					}

					size_t pos = FindPositionByType(source, i, GrammarType::GREATER_THAN_OPERATOR);
					if (pos == std::string::npos) {
						strPos = GetLineColumnByPosition(source, i);
						result.Messages.push_back({ MessageType::ERROR, "closing arrow missing", strPos.Line, strPos.Column, i });
						return result;
					}

					std::string fileName = source.substr(i, pos - i);
					for (const std::string &includePath : includePaths) {
						filePath = includePath + '/' + fileName;
						handle = std::ifstream(filePath);
						if (handle.is_open()) break;
					}

					if (!handle.is_open()) {
						strPos = GetLineColumnByPosition(source, i);
						result.Messages.push_back({ MessageType::ERROR, "failed to open `" + filePath + '`', strPos.Line, strPos.Column, i });
						return result;
					}

					i = pos;
					lexeme = GetLexeme(source, i);
					i += lexeme.Length;
				}
				else {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing file path", strPos.Line, strPos.Column, i });
					return result;
				}

				std::string fileData = ReadTextFile(handle);
				if (!preprocessFile) result.Output += fileData;
				else {
					PreprocessingResult_t subResult = Preprocess(fileData, GetDirectoryPathFromFilePath(filePath), includePaths);
					PrintMessages(subResult.Messages, filePath, fileData);
					if (!subResult.WorkCompleted) return result;

					result.Output += subResult.Output;
				}

				handle.close();
			}
			else {
				strPos = GetLineColumnByPosition(source, i);
				result.Messages.push_back({ MessageType::ERROR, "unknown preprocessor directive", strPos.Line, strPos.Column, i });
				return result;
			}
		}
		else {
			if (lexeme.Type != GrammarType::COMMENT) result.Output += source.substr(i, lexeme.Length);
			i += lexeme.Length;
		}
	}

	result.WorkCompleted = true;
	return result;
}