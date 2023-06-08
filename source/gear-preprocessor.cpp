#include <gear-preprocessor.hpp>

Gear::PreprocessingResult_t Gear::Preprocess(
	const std::string source,
	const std::string filePath,
	const std::vector<std::string> includePaths,
	std::vector<Macro_t> &macros
) {
	PreprocessingResult_t result = { false, "", {} }, tmpRes;

	Lexeme_t lexeme;
	Macro_t macro;
	size_t exprLength;
	std::string tmpStr, tmpSubStr;
	std::vector<Lexeme_t> tmpLexVec;
	size_t length = source.length();
	bool wasConditionChecked = false, wasLastConditionTrue = false;
	std::string parentDirectoryPath = GetDirectoryPathFromFilePath(filePath);
	for (size_t i = 0; i < length;) {
		lexeme = GetLexeme(source, i);

		if (lexeme.Group == GrammarGroup::UNDEFINED) {
			result.Messages.push_back({ MessageType::ERROR, "unknown lexeme found", i });
			return result;
		}
		else if (lexeme.Type == GrammarType::PREPROCESSOR_OPERATOR) {
			std::string_view preprocessorOperator = std::string_view(source).substr(i, lexeme.Length);
			if ((i += lexeme.Length) >= length) {
				result.Messages.push_back({ MessageType::ERROR, "missing preprocessor directive name", i -= lexeme.Length });
				return result;
			}

			do {
				lexeme = GetLexeme(source, i);
				if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
			} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);

			if (lexeme.Group != GrammarGroup::IDENTIFIER) {
				result.Messages.push_back({ MessageType::ERROR, "missing preprocessor directive name", i });
				return result;
			}

			std::string_view directiveName = std::string_view(source).substr(i, lexeme.Length);
			if (directiveName == "op") {
				result.Output += preprocessorOperator;
				if ((i += lexeme.Length) >= length) break;									//	skip `op`

				lexeme = GetLexeme(source, i);
				if (lexeme.Type == GrammarType::PREPROCESSOR_OPERATOR) {
					if ((i += lexeme.Length) >= length) break;								//	skip redundant lexemes
					do {
						lexeme = GetLexeme(source, i);
						if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
					} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);
				}
			}
			else if (directiveName == "join") {
				if ((i += lexeme.Length) >= length) break;									//	skip `join`
				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);			//	skip redundant lexemes
			}
			else if (directiveName == "save") {
				if ((i += lexeme.Length) >= length) {										//	skip `save`
					result.Messages.push_back({ MessageType::ERROR, "missing expression", i -= lexeme.Length });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);			//	skip redundant lexemes

				if (lexeme.Type != GrammarType::OPENING_BRACE_OPERATOR) {
					result.Messages.push_back({ MessageType::ERROR, "missing expression", i });
					return result;
				}

				exprLength = GetNestedExpressionLength(
					source,
					i,
					GrammarType::OPENING_BRACE_OPERATOR,
					GrammarType::CLOSING_BRACE_OPERATOR,
					true
				);

				if (exprLength == std::string::npos) {
					result.Messages.push_back({ MessageType::ERROR, "missing end of expression", i });
					return result;
				}

				i += lexeme.Length;															//	skip lexeme with type OPENING_BRACE_OPERATOR
				result.Output += source.substr(i, exprLength);
				i += exprLength;															//	skip expression
				lexeme = GetLexeme(source, i);
				i += lexeme.Length;															//	skip lexeme with type CLOSING_BRACE_OPERATOR
			}
			else if (directiveName == "preinc" || directiveName == "rawinc") {
				bool preprocessFile = directiveName[0] == 'p';

				if ((i += lexeme.Length) >= length) {										//	skip `preinc` or `rawinc`
					result.Messages.push_back({ MessageType::ERROR, "missing file path", i -= lexeme.Length });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);			//	skip redundant lexemes

				//	tmpStr - incFilePath

				std::ifstream handle;
				if (lexeme.Type == GrammarType::STRING_LITERAL) {
					tmpStr = parentDirectoryPath + '/' + source.substr(lexeme.Position + 1, lexeme.Length - 2);
					handle = std::ifstream(tmpStr);
					if (!handle.is_open()) {
						result.Messages.push_back({ MessageType::ERROR, "failed to open `" + tmpStr + '`', i });
						return result;
					}

					i += lexeme.Length;														//	skip "local file path"
				}
				else if (lexeme.Type == GrammarType::LESS_THAN_OPERATOR) {
					if ((i += lexeme.Length) >= length) {									//	skip lexeme with type LESS_THAN_OPERATOR
						result.Messages.push_back({ MessageType::ERROR, "missing file path", i -= lexeme.Length });
						return result;
					}

					size_t pos = FindPositionByType(source, i, GrammarType::GREATER_THAN_OPERATOR);
					if (pos == std::string::npos) {
						result.Messages.push_back({ MessageType::ERROR, "closing arrow missing", i });
						return result;
					}

					//	tmpSubStr - incFileName
					tmpSubStr = source.substr(i, pos - i);
					for (const std::string &includePath : includePaths) {
						tmpStr = includePath + '/' + tmpSubStr;
						handle = std::ifstream(tmpStr);
						if (handle.is_open()) break;
					}

					if (!handle.is_open()) {
						result.Messages.push_back({ MessageType::ERROR, "failed to open `" + tmpStr + '`', i });
						return result;
					}

					i = pos;																//	jump to lexeme with type GREATER_THAN_OPERATOR
					lexeme = GetLexeme(source, i);
					i += lexeme.Length;														//	skip lexeme with type GREATER_THAN_OPERATOR
				}
				else {
					result.Messages.push_back({ MessageType::ERROR, "missing file path", i });
					return result;
				}

				//	tmpSubStr - incFileData
				tmpSubStr = ReadTextFile(handle);
				if (!preprocessFile) result.Output += tmpSubStr;
				else {
					tmpRes = Preprocess(tmpSubStr, GetDirectoryPathFromFilePath(tmpStr), includePaths, macros);
					PrintMessages(tmpRes.Messages, tmpStr, tmpSubStr);
					if (!tmpRes.WorkCompleted) return result;
					result.Output += tmpRes.Output;
				}

				handle.close();
			}
			else if (directiveName == "def") {
				if ((i += lexeme.Length) >= length) {										//	skip `def`
					result.Messages.push_back({ MessageType::ERROR, "missing macro name", i -= lexeme.Length });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);			//	skip redundant lexemes

				size_t nextPosition = CreateMacro(source, i, macro);
				if (nextPosition == std::string::npos) {
					result.Messages.push_back({ MessageType::ERROR, "macro definition error", i });
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
				i = nextPosition;															//	skip macro definition with expression
			}
			else if (directiveName == "undef") {
				if ((i += lexeme.Length) >= length) {										//	skip `undef`
					result.Messages.push_back({ MessageType::ERROR, "missing macro name", i -= lexeme.Length });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);			//	skip redundant lexemes

				if (lexeme.Group != GrammarGroup::IDENTIFIER) {
					result.Messages.push_back({ MessageType::ERROR, "missing macro name", i });
					return result;
				}

				size_t macroIndex = GetMacroIndex(macros, lexeme.Value);
				if (macroIndex == std::string::npos) {
					result.Messages.push_back({
						MessageType::WARNING,
						"macro `" + lexeme.Value + "` cannot be undefined because it was not defined",
						i
					});
				}
				else macros.erase(macros.begin() + macroIndex);

				i += lexeme.Length;															//	skip identifier (macro name)
			}
			else if (
				directiveName == "ifdef" ||
				directiveName == "ifndef" ||
				directiveName == "if" ||
				directiveName == "elif" ||
				directiveName == "else"
			) {
				if ((i += lexeme.Length) >= length) {										//	skip `ifdef`/`ifndef`/`if`
					result.Messages.push_back({ MessageType::ERROR, "unfinished condition", i -= lexeme.Length });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);			//	skip redundant lexemes

				if (directiveName == "ifdef" || directiveName == "ifndef") {
					bool checkIsNotDefined = directiveName[2] == 'n';
					if (lexeme.Group != GrammarGroup::IDENTIFIER) {
						result.Messages.push_back({ MessageType::ERROR, "missing macro name", i });
						return result;
					}

					size_t macroIndex = GetMacroIndex(macros, lexeme.Value);
					wasLastConditionTrue = checkIsNotDefined ? macroIndex == std::string::npos : macroIndex != std::string::npos;

					if ((i += lexeme.Length) >= length) {									//	skip identifier (macro name)
						result.Messages.push_back({ MessageType::ERROR, "missing expression", i -= lexeme.Length });
						return result;
					}

					do {
						lexeme = GetLexeme(source, i);
						if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
					} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);		//	skip redundant lexemes
					
					if (lexeme.Type != GrammarType::OPENING_BRACE_OPERATOR) {
						result.Messages.push_back({ MessageType::ERROR, "missing expression", i });
						return result;
					}

					exprLength = GetNestedExpressionLength(
						source,
						i,
						GrammarType::OPENING_BRACE_OPERATOR,
						GrammarType::CLOSING_BRACE_OPERATOR
					);
					
					if (exprLength == std::string::npos) {
						result.Messages.push_back({ MessageType::ERROR, "missing end of expression", i });
						return result;
					}

					i += lexeme.Length;														//	skip lexeme with type OPENING_BRACE_OPERATOR

					if (wasLastConditionTrue) {
						tmpStr = source.substr(i, exprLength);
						tmpRes = Preprocess(tmpStr, filePath, includePaths, macros);
						PrintMessages(tmpRes.Messages, filePath, source, i);
						if (!tmpRes.WorkCompleted) return result;
						result.Output += tmpRes.Output;
					}

					i += exprLength;														//	skip expression
					lexeme = GetLexeme(source, i);
					i += lexeme.Length;														//	skip lexeme with type CLOSING_BRACE_OPERATOR
					wasConditionChecked = true;
				}
				else if (directiveName == "if" || directiveName == "elif") {
					bool checkCondition = true;
					if (directiveName[0] == 'e') {
						if (!wasConditionChecked) {
							result.Messages.push_back({ MessageType::ERROR, "missing first condition block", i });
							return result;
						}

						if (wasLastConditionTrue) checkCondition = false;
					}

					size_t exprPosition = i;
					if (lexeme.Type != GrammarType::OPENING_PARENTHESIS_OPERATOR) {
						result.Messages.push_back({ MessageType::ERROR, "missing condition", i });
						return result;
					}

					exprLength = GetNestedExpressionLength(
						source,
						i,
						GrammarType::OPENING_PARENTHESIS_OPERATOR,
						GrammarType::CLOSING_PARENTHESIS_OPERATOR
					);

					if (exprLength == std::string::npos) {
						result.Messages.push_back({ MessageType::ERROR, "missing end of condition", i });
						return result;
					}

					i += lexeme.Length;														//	skip lexeme with type OPENING_PARENTHESIS_OPERATOR

					//	tmpStr - condition
					tmpStr = "";
					for (size_t j = i + exprLength; i < j;) {								//	skip condition
						lexeme = GetLexeme(source, i);
						tmpStr += lexeme.Value;
						i += lexeme.Length;
					}

					tmpRes = Preprocess(tmpStr, filePath, includePaths, macros);
					PrintMessages(tmpRes.Messages, filePath, source, exprPosition);
					if (!tmpRes.WorkCompleted) return result;
					tmpStr = tmpRes.Output;
					wasLastConditionTrue = checkCondition ? EvaluateRpnExpression(InfixToRpnExpression(Lex(tmpStr))) == true : false;
					lexeme = GetLexeme(source, i);
					i += lexeme.Length;														//	skip lexeme with type CLOSING_PARENTHESIS_OPERATOR

					do {
						lexeme = GetLexeme(source, i);
						if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
					} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);		//	skip redundant lexemes

					if (lexeme.Type != GrammarType::OPENING_BRACE_OPERATOR) {
						result.Messages.push_back({ MessageType::ERROR, "missing expression", i });
						return result;
					}

					exprPosition = i;
					exprLength = GetNestedExpressionLength(
						source,
						i,
						GrammarType::OPENING_BRACE_OPERATOR,
						GrammarType::CLOSING_BRACE_OPERATOR
					);

					if (exprLength == std::string::npos) {
						result.Messages.push_back({ MessageType::ERROR, "missing end of expression", i });
						return result;
					}

					i += lexeme.Length;														//	skip lexeme with type OPENING_BRACE_OPERATOR

					if (wasLastConditionTrue) {
						tmpStr = source.substr(i, exprLength);
						tmpRes = Preprocess(tmpStr, filePath, includePaths, macros);
						PrintMessages(tmpRes.Messages, filePath, source, exprPosition);
						if (!tmpRes.WorkCompleted) return result;
						result.Output += tmpRes.Output;
					}

					i += exprLength;														//	skip expression
					lexeme = GetLexeme(source, i);
					i += lexeme.Length;														//	skip lexeme with type CLOSING_BRACE_OPERATOR
					wasConditionChecked = true;
				}
				else if (directiveName == "else") {
					if (!wasConditionChecked) {
						result.Messages.push_back({ MessageType::ERROR, "missing previous condition block", i });
						return result;
					}

					if (lexeme.Type != GrammarType::OPENING_BRACE_OPERATOR) {
						result.Messages.push_back({ MessageType::ERROR, "missing expression", i });
						return result;
					}

					size_t exprPosition = i;
					exprLength = GetNestedExpressionLength(
						source,
						i,
						GrammarType::OPENING_BRACE_OPERATOR,
						GrammarType::CLOSING_BRACE_OPERATOR
					);

					if (exprLength == std::string::npos) {
						result.Messages.push_back({ MessageType::ERROR, "missing end of expression", i });
						return result;
					}

					i += lexeme.Length;														//	skip lexeme with type OPENING_BRACE_OPERATOR

					if (!wasLastConditionTrue) {
						tmpStr = source.substr(i, exprLength);
						tmpRes = Preprocess(tmpStr, filePath, includePaths, macros);
						PrintMessages(tmpRes.Messages, filePath, source, exprPosition);
						if (!tmpRes.WorkCompleted) return result;
						result.Output += tmpRes.Output;
					}

					i += exprLength;														//	skip expression
					lexeme = GetLexeme(source, i);
					i += lexeme.Length;														//	skip lexeme with type CLOSING_BRACE_OPERATOR
					wasConditionChecked = false;
				}
			}
			else if (directiveName == "defined") {
				if ((i += lexeme.Length) >= length) {										//	skip `defined`
					result.Messages.push_back({ MessageType::ERROR, "missing macro name", i -= lexeme.Length });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);			//	skip redundant lexemes

				if (lexeme.Group != GrammarGroup::IDENTIFIER) {
					result.Messages.push_back({ MessageType::ERROR, "missing macro name", i });
					return result;
				}

				result.Output += GetMacroIndex(macros, lexeme.Value) == std::string::npos ? '0' : '1';

				i += lexeme.Length;															//	skip identifier (macro name)
			}
			else {
				result.Messages.push_back({ MessageType::ERROR, "unknown preprocessor directive", i });
				return result;
			}
		}
		else if (lexeme.Type == GrammarType::IDENTIFIER) {
			//	tmpStr - identifier name
			tmpStr = source.substr(lexeme.Position, lexeme.Length);
			size_t macroIndex = GetMacroIndex(macros, tmpStr);
			if (macroIndex == std::string::npos) {
				result.Output += tmpStr;
				i += lexeme.Length;
				continue;
			}

			if (GetLexemeIndex(macros[macroIndex].Expression, tmpStr) != std::string::npos) {
				result.Messages.push_back({ MessageType::ERROR, "macro name found in its expression", i });
				return result;
			}

			if (macros[macroIndex].IsFunction) {
				if ((i += lexeme.Length) >= length) {
					result.Messages.push_back({ MessageType::ERROR, "missing function macro arguments", i -= lexeme.Length });
					return result;
				}

				do {
					lexeme = GetLexeme(source, i);
					if (lexeme.Group == GrammarGroup::REDUNDANT) i += lexeme.Length;
				} while (i < length && lexeme.Group == GrammarGroup::REDUNDANT);

				if (lexeme.Type != GrammarType::OPENING_PARENTHESIS_OPERATOR) {
					result.Messages.push_back({ MessageType::ERROR, "missing function macro arguments", i });
					return result;
				}

				std::vector<std::vector<Lexeme_t> > arguments;
				if (!GetArgumentsFromNestedExpression(source, i, arguments)) {
					result.Messages.push_back({ MessageType::ERROR, "incorrect use of a macro", i });
					return result;
				}

				exprLength = GetExpressionLength(
					source,
					i,
					GrammarType::OPENING_PARENTHESIS_OPERATOR,
					GrammarType::CLOSING_PARENTHESIS_OPERATOR
				);

				std::vector<Lexeme_t> exprResult;
				if (
					exprLength == std::string::npos ||
					!ExpandExpression(source, macros[macroIndex].Expression, arguments, macros[macroIndex].Parameters, macros, exprResult)
				) {
					result.Messages.push_back({ MessageType::ERROR, "incorrect use of a macro", i });
					return result;
				}

				for (const Lexeme_t &lex : exprResult) {
					if (lex.Length == 0) result.Output += std::to_string(lex.Position);
					else result.Output += lex.Value;
				}
				i += exprLength;
			}
			else {
				std::vector<Lexeme_t> exprResult;
				if (!ExpandExpression(source, macros[macroIndex].Expression, {}, macros[macroIndex].Parameters, macros, exprResult)) {
					result.Messages.push_back({ MessageType::ERROR, "incorrect use of a macro", i });
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

	size_t exprLength, position = namePosition;
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

		exprLength = GetExpressionLength(
			source,
			position,
			GrammarType::OPENING_PARENTHESIS_OPERATOR,
			GrammarType::CLOSING_PARENTHESIS_OPERATOR
		);

		if (exprLength == std::string::npos) return std::string::npos;
		position += exprLength;
	}

	do {
		lexeme = GetLexeme(source, position);
		if (lexeme.Group == GrammarGroup::REDUNDANT) position += lexeme.Length;
	} while (lexeme.Group == GrammarGroup::REDUNDANT);

	if (lexeme.Type != GrammarType::OPENING_BRACE_OPERATOR) return std::string::npos;

	exprLength = GetNestedExpressionLength(
		source,
		position,
		GrammarType::OPENING_BRACE_OPERATOR,
		GrammarType::CLOSING_BRACE_OPERATOR
	);

	if (exprLength == std::string::npos) return std::string::npos;

	position += lexeme.Length;
	size_t closingLimiterPosition = position + exprLength;

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