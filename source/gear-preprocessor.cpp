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

				size_t macroIndex = GetMacroIndex(macros, lexeme.Value);
				if (macroIndex == std::string::npos) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({
						MessageType::WARNING,
						"macro `" + lexeme.Value + "` cannot be undefined because it was not defined",
						strPos.Line,
						strPos.Column,
						i
					});
				}
				else macros.erase(macros.begin() + macroIndex);

				i += lexeme.Length;
			}
			else {
				strPos = GetLineColumnByPosition(source, i);
				result.Messages.push_back({ MessageType::ERROR, "unknown preprocessor directive", strPos.Line, strPos.Column, i });
				return result;
			}
		}
		else if (lexeme.Type == GrammarType::IDENTIFIER) {
			std::string name = source.substr(lexeme.Position, lexeme.Length);
			size_t macroIndex = GetMacroIndex(macros, name);
			if (macroIndex == std::string::npos) {
				result.Output += name;
				i += lexeme.Length;
				continue;
			}

			if (GetLexemeIndex(macros[macroIndex].Expression, name) != std::string::npos) {
				strPos = GetLineColumnByPosition(source, i);
				result.Messages.push_back({ MessageType::ERROR, "macro name found in its expression", strPos.Line, strPos.Column, i });
				return result;
			}

			if (macros[macroIndex].IsFunction) {
				if ((i += lexeme.Length) >= length) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing function macro arguments", strPos.Line, strPos.Column, i });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);

				if (lexeme.Type != GrammarType::OPENING_PARENTHESIS_OPERATOR) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "missing function macro arguments", strPos.Line, strPos.Column, i });
					return result;
				}

				std::vector<std::vector<Lexeme_t> > arguments;
				if (!GetArgumentsFromNestedExpression(source, i, arguments)) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "incorrect use of a macro", strPos.Line, strPos.Column, i });
					return result;
				}

				size_t nestingLength = GetNestingLength(
					source,
					i,
					GrammarType::OPENING_PARENTHESIS_OPERATOR,
					GrammarType::CLOSING_PARENTHESIS_OPERATOR,
					false
				);

				std::vector<Lexeme_t> exprResult;
				if (
					nestingLength == std::string::npos ||
					!ExpandExpression(source, macros[macroIndex].Expression, arguments, macros[macroIndex].Parameters, macros, exprResult)
				) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "incorrect use of a macro", strPos.Line, strPos.Column, i });
					return result;
				}

				for (const Lexeme_t &lex : exprResult) {
					if (lex.Length == 0) result.Output += std::to_string(lex.Position);
					else result.Output += lex.Value;
				}
				i += nestingLength;
			}
			else {
				std::vector<Lexeme_t> exprResult;
				if (!ExpandExpression(source, macros[macroIndex].Expression, {}, macros[macroIndex].Parameters, macros, exprResult)) {
					strPos = GetLineColumnByPosition(source, i);
					result.Messages.push_back({ MessageType::ERROR, "incorrect use of a macro", strPos.Line, strPos.Column, i });
					return result;
				}

				for (const Lexeme_t &lex : exprResult) result.Output += lex.Value;
				i += lexeme.Length;
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
	result.Name = lexeme.Value;
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
					if (lexVec[i].Type == GrammarType::ELLIPSIS_OPERATOR) break;
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
		if (lexeme.Value == result.Name) return std::string::npos;
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

bool Gear::ExpandExpression(
	const std::string source,
	const std::vector<Lexeme_t> expression,
	const std::vector<std::vector<Lexeme_t> > arguments,
	const std::vector<Lexeme_t> parameters,
	const std::vector<Macro_t> macros,
	std::vector<Lexeme_t> &result
) {
	std::string value;
	size_t count = expression.size();
	size_t argsCount = arguments.size();
	for (size_t i = 0; i < count; i++) {
		value = expression[i].Value;
		if (expression[i].Group == GrammarGroup::IDENTIFIER) {
			size_t index = GetMacroIndex(macros, value);
			if (index == std::string::npos) {
				index = GetLexemeIndex(parameters, value);
				if (index == std::string::npos) {
					result.push_back(expression[i]);
					continue;
				}

				if (index >= argsCount) return false;
				if (!ExpandExpression(source, arguments[index], arguments, parameters, macros, result)) return false;
				continue;
			}

			if (macros[index].IsFunction) {
				if ((i += 1) >= count) return false;
				while (i < count && expression[i].Group == GrammarGroup::REDUNDANT) i += 1;
				if (i >= count || expression[i].Type != GrammarType::OPENING_PARENTHESIS_OPERATOR) return false;

				std::vector<std::vector<Lexeme_t> > tmpArguments;
				if (!GetArgumentsFromNestedExpression(expression, i, tmpArguments)) return false;

				std::vector<Lexeme_t> tmpArg;
				std::vector<std::vector<Lexeme_t> > subArguments;
				for (const std::vector<Lexeme_t> &subVec : tmpArguments) {
					if (!ExpandExpression(source, subVec, arguments, parameters, macros, tmpArg)) return false;
					subArguments.push_back(tmpArg);
					tmpArg.clear();
				}

				if (!ExpandExpression(source, macros[index].Expression, subArguments, macros[index].Parameters, macros, result)) return false;

				size_t level = 0;
				while (i < count) {
					if (expression[i].Type == GrammarType::OPENING_PARENTHESIS_OPERATOR) {
						level += 1;
					}
					else if (expression[i].Type == GrammarType::CLOSING_PARENTHESIS_OPERATOR) {
						if (!level) return false;
						if (level == 1) break;
						level -= 1;
					}

					i += 1;
				}
			}
			else if (!ExpandExpression(source, macros[index].Expression, arguments, parameters, macros, result)) return false;
		}
		else if (expression[i].Type == GrammarType::ELLIPSIS_OPERATOR) {
			size_t index = GetLexemeIndex(parameters, value);
			if (index >= argsCount) return false;

			Lexeme_t lexeme = { GrammarGroup::UNDEFINED, GrammarType::UNDEFINED, 0, 0, "" };
			for (; index < argsCount; index++) {
				if (!ExpandExpression(source, arguments[index], arguments, parameters, macros, result)) return false;
				if (index < argsCount - 1) {
					if (lexeme.Group == GrammarGroup::UNDEFINED) lexeme = GetLexeme(
						source, arguments[index].back().Position + arguments[index].back().Length
					);

					result.push_back(lexeme);
				}
			}
		}
		else if (expression[i].Type == GrammarType::PREPROCESSOR_OPERATOR) {
			if (parameters.back().Type != GrammarType::ELLIPSIS_OPERATOR) return false;

			size_t index = parameters.size() - 1;
			if (i < count - 1 && expression[i + 1].Type == GrammarType::DEC_LITERAL) {
				i += 1;

				if (index >= argsCount) return false;

				index += std::stoul(source.substr(expression[i].Position, expression[i].Length));
				if (index >= argsCount) return false;

				result.insert(result.end(), arguments[index].begin(), arguments[index].end());
			}
			else {
				result.push_back({
					GrammarGroup::LITERAL,
					GrammarType::DEC_LITERAL,
					index >= argsCount ? 0 : argsCount - index,
					0,
					""
				});
			}
		}
		else if (expression[i].Type != GrammarType::COMMENT) result.push_back(expression[i]);
	}

	return true;
}