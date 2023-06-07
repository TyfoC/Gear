#include <gear-preprocessor.hpp>

Gear::PreprocessingResult_t Gear::Preprocess(
	const std::string source,
	const std::string srcFileDirectoryPath,
	const std::vector<std::string> includePaths,
	std::vector<Macro_t> &macros
) {
	PreprocessingResult_t result = { false, "", {} };

	Lexeme_t lexeme;
	Macro_t macro;
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
					PreprocessingResult_t subResult = Preprocess(fileData, GetDirectoryPathFromFilePath(filePath), includePaths, macros);
					PrintMessages(subResult.Messages, filePath, fileData);
					if (!subResult.WorkCompleted) return result;

					result.Output += subResult.Output;
				}

				handle.close();
			}
			else if (directiveName == "def") {
				if ((i += lexeme.Length) >= length) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing macro name", strPos.Line, strPos.Column, i });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);

				size_t nextPosition = CreateMacro(source, i, macro);
				if (nextPosition == std::string::npos) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "macro definition error", strPos.Line, strPos.Column, i });
					return result;
				}

				size_t macroIndex = GetMacroIndex(macros, macro.Name);
				if (macroIndex == std::string::npos) macros.push_back(macro);
				else {
					macros[macroIndex].Parameters = macro.Parameters;
					macros[macroIndex].Expression = macro.Expression;
					macros[macroIndex].IsFunction = macro.IsFunction;
				}

				macro = {};
				i = nextPosition;
			}
			else if (directiveName == "undef") {
				if ((i += lexeme.Length) >= length) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing macro name", strPos.Line, strPos.Column, i });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);

				if (lexeme.Group != GrammarGroup::IDENTIFIER) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing macro name", strPos.Line, strPos.Column, i });
					return result;
				}

				std::string macroName = source.substr(lexeme.Position, lexeme.Length);
				size_t macroIndex = GetMacroIndex(macros, macroName);
				if (macroIndex == std::string::npos) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({
						MessageType::WARNING,
						"macro `" + macroName + "` cannot be undefined because it was not defined",
						strPos.Line,
						strPos.Column,
						i
					});
				}

				i += lexeme.Length;
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

size_t Gear::CreateMacro(const std::string source, size_t namePosition, Macro_t &result) {
	size_t length = source.length();
	if (namePosition >= length) return std::string::npos;

	Lexeme_t lexeme = GetLexeme(source, namePosition);
	if (lexeme.Group != GrammarGroup::IDENTIFIER) return std::string::npos;

	size_t position = namePosition;
	result.Name = source.substr(position, lexeme.Length);
	position += lexeme.Length;
	lexeme = GetLexeme(source, position);
	result.IsFunction = lexeme.Type == GrammarType::OPENING_PARENTHESIS_OPERATOR;
	if (result.IsFunction) {
		size_t lexemesCount;
		bool identifierAdded;
		std::vector<std::vector<Lexeme_t> > parameters;
		if (!GetArgumentsFromNestedExpression(source, position, parameters)) return std::string::npos;

		for (const std::vector<Lexeme_t> &lexVec : parameters) {
			lexemesCount = lexVec.size();
			identifierAdded = false;
			for (size_t i = 0; i < lexemesCount; i++) {
				if (lexVec[i].Group == GrammarGroup::IDENTIFIER || lexVec[i].Type == GrammarType::ELLIPSIS_OPERATOR) {
					if (identifierAdded) return std::string::npos;
					result.Parameters.push_back(lexVec[i]);
					identifierAdded = true;
				}
				else if (lexVec[i].Group != GrammarGroup::REDUNDANT) return std::string::npos;
			}
		}

		size_t nestingLength = GetNestingLength(
			source,
			position,
			GrammarType::OPENING_PARENTHESIS_OPERATOR,
			GrammarType::CLOSING_PARENTHESIS_OPERATOR
		);

		if (nestingLength == std::string::npos) return std::string::npos;
		position += nestingLength;
	}

	do {
		lexeme = GetLexeme(source, position);
		if (lexeme.Group == GrammarGroup::REDUNDANT) position += lexeme.Length;
	} while (lexeme.Group == GrammarGroup::REDUNDANT);

	if (lexeme.Type != GrammarType::OPENING_BRACE_OPERATOR) return std::string::npos;

	size_t nestedExprLen = GetNestedExpressionLength(
		source,
		position,
		GrammarType::OPENING_BRACE_OPERATOR,
		GrammarType::CLOSING_BRACE_OPERATOR, false
	);

	if (nestedExprLen == std::string::npos) return std::string::npos;

	position += lexeme.Length;
	size_t closingLimiterPosition = position + nestedExprLen;

	while (position < closingLimiterPosition) {
		lexeme = GetLexeme(source, position);
		if (lexeme.Type != GrammarType::COMMENT) result.Expression.push_back(lexeme);
		position += lexeme.Length;
	}

	lexeme = GetLexeme(source, position);
	return position + lexeme.Length;
}

size_t Gear::GetMacroIndex(const std::vector<Macro_t> macros, const std::string name) {
	size_t count = macros.size();
	for (size_t i = 0; i < count; i++) if (macros[i].Name == name) return i;
	return std::string::npos;
}