#include <gear-preprocessor.hpp>

Gear::PreprocessingResult Gear::Preprocess(
	const std::string source,
	const std::string filePath,
	const std::vector<std::string> includePaths,
	std::vector<Macro> &macros
) {
	PreprocessingResult result = { false, "", {} }, tmpRes;

	Token token;
	Macro macro;
	Scanner scanner(source);
	std::vector<Token> tmpExpr;
	std::string tmpStr, tmpSubStr;
	bool isConditionChecked = false, isConditionTrue = false;
	std::string parentDirPath = GetDirectoryPathFromFilePath(filePath);
	while (!scanner.IsAtEnd()) {
		token = scanner.ScanNext();
		if (token.Group == Token::GROUP_UNDEFINED) {
			result.Messages.push_back({ Message::TYPE_ERROR, "unexpected expression", token.Position });
			return result;
		}
		else if (token.Type == Token::TYPE_PREPROCESSOR_OPERATOR) {
			tmpStr = token.Lexeme;										//	preprocessor directive lexeme
			if (scanner.IsAtEnd()) {
				result.Messages.push_back({ Message::TYPE_ERROR, "missing preprocessor directive name", token.Position });
				return result;
			}

			do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
			if (token.Group != Token::GROUP_IDENTIFIER) {
				result.Messages.push_back({ Message::TYPE_ERROR, "missing preprocessor directive name", token.Position });
				return result;
			}

			tmpSubStr = token.Lexeme;									//	directive name
			if (tmpSubStr == "op") {
				result.Output += tmpStr;
				token = scanner.Scan();
				if (token.Type == Token::TYPE_PREPROCESSOR_OPERATOR) {
					scanner.SetPosition(token.Position + token.Lexeme.length());
					do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
					scanner.SetPosition(token.Position);
				}
			}
			else if (tmpSubStr == "join") {
				do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
				scanner.SetPosition(token.Position);
			}
			else if (tmpSubStr == "save") {
				do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
				if (token.Type != Token::TYPE_OPENING_BRACE_OPERATOR) {
					result.Messages.push_back({ Message::TYPE_ERROR, "missing expression", token.Position });
					return result;
				}

				scanner.SetPosition(token.Position);
				if (!scanner.ScanExpressionNext(tmpExpr)) {
					tmpExpr.clear();
					result.Messages.push_back({ Message::TYPE_ERROR, "missing expression end", token.Position });
					return result;
				}

				for (const Token &element : tmpExpr) result.Output += element.Lexeme;
				tmpExpr.clear();
			}
			else if (tmpSubStr == "preinc" || tmpSubStr == "rawinc") {
				bool preprocessFile = tmpSubStr[0] == 'p';
				do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);

				//	tmpStr - file path

				std::ifstream handle;
				if (token.Type == Token::TYPE_STRING_LITERAL) {
					tmpStr = parentDirPath + '/' + token.Lexeme.substr(1, token.Lexeme.length() - 2);
					handle = std::ifstream(tmpStr);
				}
				else if (token.Type == Token::TYPE_LESS_THAN_OPERATOR) {
					scanner.SetPosition(token.Position);
					if (!scanner.ScanExpressionNext(tmpExpr)) {
						tmpExpr.clear();
						result.Messages.push_back({ Message::TYPE_ERROR, "missing filename end", token.Position });
						return result;
					}

					tmpSubStr = "";
					for (const Token &element : tmpExpr) tmpSubStr += element.Lexeme;
					tmpExpr.clear();

					for (const std::string &includePath : includePaths) {
						tmpStr = includePath + '/' + tmpSubStr;
						handle = std::ifstream(tmpStr);
						if (handle.is_open()) break;
					}

					if (!handle.is_open()) {
						result.Messages.push_back({
							Message::TYPE_ERROR,
							"could not find file `" + tmpSubStr + "` in the specified directories",
							token.Position
						});
						return result;
					}
				}

				//	tmpSubStr - file data
				tmpSubStr = ReadTextFile(handle);
				if (preprocessFile) {
					tmpRes = Preprocess(tmpSubStr, tmpStr, includePaths, macros);
					PrintMessages(tmpRes.Messages, tmpStr, tmpSubStr);
					if (!tmpRes.IsWorkCompleted) return result;
					result.Output += tmpRes.Output;
				}
				else result.Output += tmpSubStr;
			}
			else if (tmpSubStr == "def") {
				do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
				if (token.Type != Token::TYPE_IDENTIFIER) {
					result.Messages.push_back({ Message::TYPE_ERROR, "missing macro name", token.Position });
					return result;
				}

				size_t macroIndex = GetMacroIndex(macros, token.Lexeme);
				scanner.SetPosition(token.Position);
				if (!CreateMacro(scanner, macro)) {
					result.Messages.push_back({ Message::TYPE_ERROR, "macro definition error", token.Position });
					return result;
				}

				if (macroIndex == std::string::npos) macros.push_back(macro);
				else macros[macroIndex] = macro;

				macro = {};
			}
			else if (tmpSubStr == "undef") {
				do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
				if (token.Type != Token::TYPE_IDENTIFIER) {
					result.Messages.push_back({ Message::TYPE_ERROR, "missing macro name", token.Position });
					return result;
				}

				size_t macroIndex = GetMacroIndex(macros, token.Lexeme);
				if (macroIndex == std::string::npos) {
					result.Messages.push_back({ Message::TYPE_WARNING, "macro `" + token.Lexeme + "` was not defined", token.Position });
					return result;
				}
				else macros.erase(macros.begin() + macroIndex);
			}
			else if (
				tmpSubStr == "ifdef" ||
				tmpSubStr == "ifndef" ||
				tmpSubStr == "if" ||
				tmpSubStr == "elif" ||
				tmpSubStr == "else"
			) {
				do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
				if (tmpSubStr == "ifdef" || tmpSubStr == "ifndef") {
					bool checkIsDefined = tmpSubStr[2] == 'd';
					if (token.Group != Token::GROUP_IDENTIFIER) {
						result.Messages.push_back({ Message::TYPE_ERROR, "missing macro name", token.Position });
						return result;
					}

					size_t macroIndex = GetMacroIndex(macros, token.Lexeme);
					isConditionTrue = checkIsDefined ? macroIndex != std::string::npos : macroIndex == std::string::npos;

					do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
					if (token.Type != Token::TYPE_OPENING_BRACE_OPERATOR) {
						result.Messages.push_back({ Message::TYPE_ERROR, "missing expression", token.Position });
						return result;
					}

					size_t position = scanner.GetPosition();
					scanner.SetPosition(token.Position);
					if (!scanner.ScanExpressionNext(tmpExpr)) {
						result.Messages.push_back({ Message::TYPE_ERROR, "missing expression end", token.Position });
						return result;
					}

					if (isConditionTrue) {
						tmpStr = "";
						for (const Token &element : tmpExpr) tmpStr += element.Lexeme;
						tmpRes = Preprocess(tmpStr, filePath, includePaths, macros);
						PrintMessages(tmpRes.Messages, filePath, source, position);
						if (!tmpRes.IsWorkCompleted) return result;
						result.Output += tmpRes.Output;
					}
					
					tmpExpr.clear();
					isConditionChecked = true;
				}
				else if (tmpSubStr == "if" || tmpSubStr == "elif") {
					bool checkCondition = true;
					if (tmpSubStr[0] == 'e') {
						if (!isConditionChecked) {
							result.Messages.push_back({ Message::TYPE_ERROR, "mmissing previous condition block", token.Position });
							return result;
						}

						if (isConditionTrue) checkCondition = false;
					}

					if (token.Type != Token::TYPE_OPENING_PARENTHESIS_OPERATOR) {
						result.Messages.push_back({ Message::TYPE_ERROR, "missing condition", token.Position });
						return result;
					}

					size_t position = token.Position;
					scanner.SetPosition(token.Position);
					if (!scanner.ScanExpressionNext(tmpExpr)) {
						result.Messages.push_back({ Message::TYPE_ERROR, "missing condition end", token.Position });
						return result;
					}

					tmpStr = "";
					for (const Token &element : tmpExpr) tmpStr += element.Lexeme;
					tmpExpr.clear();
					tmpRes = Preprocess(tmpStr, filePath, includePaths, macros);
					PrintMessages(tmpRes.Messages, filePath, source, position);
					if (!tmpRes.IsWorkCompleted) return result;
					isConditionTrue = checkCondition ? MathEvaluatePostfixExpression(
						MathInfixToPostfixExpression(Scanner(tmpRes.Output).ScanToEnd())
					) != 0 : false;

					do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
					if (token.Type != Token::TYPE_OPENING_BRACE_OPERATOR) {
						result.Messages.push_back({ Message::TYPE_ERROR, "missing expression", token.Position });
						return result;
					}

					position = token.Position;
					scanner.SetPosition(token.Position);
					if (!scanner.ScanExpressionNext(tmpExpr)) {
						result.Messages.push_back({ Message::TYPE_ERROR, "missing expression end", token.Position });
						return result;
					}

					if (isConditionTrue) {
						tmpStr = "";
						for (const Token &element : tmpExpr) tmpStr += element.Lexeme;
						tmpRes = Preprocess(tmpStr, filePath, includePaths, macros);
						PrintMessages(tmpRes.Messages, filePath, source, position);
						if (!tmpRes.IsWorkCompleted) return result;
						result.Output += tmpRes.Output;
					}

					tmpExpr.clear();
					isConditionChecked = true;
				}
				else if (tmpSubStr == "else") {
					if (!isConditionChecked) {
						result.Messages.push_back({ Message::TYPE_ERROR, "mmissing previous condition block", token.Position });
						return result;
					}

					if (token.Type != Token::TYPE_OPENING_BRACE_OPERATOR) {
						result.Messages.push_back({ Message::TYPE_ERROR, "missing expression", token.Position });
						return result;
					}
					
					size_t position = token.Position;
					scanner.SetPosition(token.Position);
					if (!scanner.ScanExpressionNext(tmpExpr)) {
						result.Messages.push_back({ Message::TYPE_ERROR, "missing expression end", token.Position });
						return result;
					}

					if (!isConditionTrue) {
						tmpStr = "";
						for (const Token &element : tmpExpr) tmpStr += element.Lexeme;
						tmpRes = Preprocess(tmpStr, filePath, includePaths, macros);
						PrintMessages(tmpRes.Messages, filePath, source, position);
						if (!tmpRes.IsWorkCompleted) return result;
						result.Output += tmpRes.Output;
					}

					tmpExpr.clear();
					isConditionChecked = false;
				}
			}
			else if (tmpSubStr == "defined") {
				do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
				if (token.Group != Token::GROUP_IDENTIFIER) {
					result.Messages.push_back({ Message::TYPE_ERROR, "missing macro name", token.Position });
					return result;
				}

				size_t macroIndex = GetMacroIndex(macros, token.Lexeme);
				result.Output += macroIndex == std::string::npos ? '0' : '1';
			}
			else {
				result.Messages.push_back({ Message::TYPE_ERROR, "unknown preprocessor directive", token.Position });
				return result;
			}
		}
		else if (token.Type == Token::TYPE_IDENTIFIER) {
			size_t macroIndex = GetMacroIndex(macros, token.Lexeme);
			if (macroIndex == std::string::npos) {
				result.Output += token.Lexeme;
				continue;
			}

			if (macros[macroIndex].IsFunction) {
				do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
				if (token.Type != Token::TYPE_OPENING_PARENTHESIS_OPERATOR) {
					result.Messages.push_back({ Message::TYPE_ERROR, "missing macro arguments", token.Position });
					return result;
				}

				scanner.SetPosition(token.Position);

				std::vector<std::vector<Token> > tmpArgs;
				if (!scanner.ScanSeparatedExpressionsNext(Token::TYPE_COMMA_OPERATOR, tmpArgs)) {
					result.Messages.push_back({ Message::TYPE_ERROR, "missing macro arguments end", token.Position });
					return result;
				}

				if (!ExpandExpression(macros[macroIndex].Expression, macros[macroIndex].Parameters, tmpArgs, macros, tmpExpr)) {
					result.Messages.push_back({ Message::TYPE_ERROR, "incorrect use of macro", token.Position });
					return result;
				}

				for (const Token &element : tmpExpr) {
					if (element.Lexeme == "") result.Output += std::to_string(element.Position);		//	arg index/args count
					else result.Output += element.Lexeme;
				}
			}
			else {
				if (!ExpandExpression(macros[macroIndex].Expression, macros[macroIndex].Parameters, {}, macros, tmpExpr)) {
					result.Messages.push_back({ Message::TYPE_ERROR, "incorrect use of macro", token.Position });
					return result;
				}

				for (const Token &element : tmpExpr) {
					if (element.Lexeme == "") result.Output += std::to_string(element.Position);		//	arg index/args count
					else result.Output += element.Lexeme;
				}
			}

			tmpExpr.clear();
		}
		else if (token.Type != Token::TYPE_COMMENT) result.Output += token.Lexeme;
	}

	result.IsWorkCompleted = true;
	return result;
}

size_t Gear::GetMacroIndex(const std::vector<Macro> &macros, const std::string &name) {
	size_t count = macros.size();
	for (size_t i = 0; i < count; i++) if (macros[i].Name == name) return i;
	return std::string::npos;
}

bool Gear::CreateMacro(
	Scanner &scanner,
	Macro &result
) {
	if (scanner.IsAtEnd()) return false;
	
	Token token = scanner.ScanNext();
	if (token.Group != Token::GROUP_IDENTIFIER) return false;

	result.Name = token.Lexeme;
	do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);

	if (token.Type == Token::TYPE_OPENING_PARENTHESIS_OPERATOR) {
		result.IsFunction = true;
		scanner.SetPosition(token.Position);

		std::vector<std::vector<Token> > parameters;
		if (!scanner.ScanSeparatedExpressionsNext(Token::TYPE_COMMA_OPERATOR, parameters, true)) return false;
		for (const std::vector<Token> &element : parameters) {
			if (element.size() != 1) return false;
			result.Parameters.push_back(element[0]);
		}

		do { token = scanner.ScanNext(); } while (token.Group == Token::GROUP_REDUNDANT);
	}
	else result.IsFunction = false;

	if (token.Type != Token::TYPE_OPENING_BRACE_OPERATOR) return false;
	scanner.SetPosition(token.Position);

	if (!scanner.ScanExpressionNext(result.Expression)) return false;
	if (GetTokenIndex(result.Expression, result.Name) != std::string::npos) return false;		//	macro name in expression
	return true;
}

bool Gear::ExpandExpression(
	const std::vector<Token> &expression,
	const std::vector<Token> &parameters,
	const std::vector<std::vector<Token> > &arguments,
	const std::vector<Macro> &macros,
	std::vector<Token> &result
) {
	size_t commaIndex = GetTokenTemplateIndex(Token::TYPE_COMMA_OPERATOR);
	std::vector<TokenTemplate> tokenTemplates = GetTokenTemplates();
	if (commaIndex == std::string::npos) return false;

	size_t paramsCount = parameters.size();
	size_t argsCount = arguments.size();
	size_t count = expression.size();
	for (size_t i = 0; i < count; i++) {
		if (expression[i].Group == Token::GROUP_IDENTIFIER) {
			size_t index = GetMacroIndex(macros, expression[i].Lexeme);
			if (index == std::string::npos) {
				index = GetTokenIndex(parameters, expression[i].Lexeme);
				if (index == std::string::npos) {
					result.push_back(expression[i]);
					continue;
				}

				if (index >= argsCount) return false;
				if (!ExpandExpression(arguments[index], parameters, arguments, macros, result)) return false;
				continue;
			}

			if (macros[index].IsFunction) {
				if ((i += 1) >= count) return false;
				while (i < count && expression[i].Group == Token::GROUP_REDUNDANT) i += 1;
				if (i >= count || expression[i].Type != Token::TYPE_OPENING_PARENTHESIS_OPERATOR) return false;

				std::vector<std::vector<Token> > tmpArgs;
				if (!Scanner::ScanSeparatedExpressions(expression, i, Token::TYPE_COMMA_OPERATOR, tmpArgs)) return false;

				std::vector<Token> tmpArg;
				std::vector<std::vector<Token> > subArgs;
				for (const std::vector<Token> &element : tmpArgs) {
					if (!ExpandExpression(element, parameters, arguments, macros, tmpArg)) return false;
					subArgs.push_back(tmpArg);
					tmpArg.clear();
				}

				if (!ExpandExpression(macros[index].Expression, macros[index].Parameters, subArgs, macros, result)) return false;

				size_t level = 0;
				while (i < count) {
					if (expression[i].Type == Token::TYPE_OPENING_PARENTHESIS_OPERATOR) {
						level += 1;
					}
					else if (expression[i].Type == Token::TYPE_CLOSING_PARENTHESIS_OPERATOR) {
						if (!level) return false;
						if (level == 1) break;
						level -= 1;
					}

					i += 1;
				}
			}
			else if (!ExpandExpression(macros[index].Expression, parameters, arguments, macros, result)) return false;
		}
		else if (expression[i].Type == Token::TYPE_ELLIPSIS_OPERATOR) {
			size_t index = GetTokenIndex(parameters, expression[i].Lexeme);
			if (index >= argsCount) return false;

			for (; index < argsCount; index++) {
				if (!ExpandExpression(arguments[index], parameters, arguments, macros, result)) return false;
				if (index < argsCount - 1) {														//	append comma operator
					result.push_back({
						tokenTemplates[commaIndex].Group,
						tokenTemplates[commaIndex].Type,
						arguments[index].back().Position + arguments[index].back().Lexeme.length(),
						tokenTemplates[commaIndex].Pattern,
						0
					});
				}
			}
		}
		else if (expression[i].Type == Token::TYPE_PREPROCESSOR_OPERATOR) {
			if (parameters.back().Type != Token::TYPE_ELLIPSIS_OPERATOR) return false;

			size_t index = paramsCount - 1;
			if (i < count - 1 && expression[i + 1].Type == Token::TYPE_DEC_LITERAL) {
				i += 1;
				index += *(size_t*)expression[i].Literal;
				if (index >= argsCount) return false;
				result.insert(result.end(), arguments[index].begin(), arguments[index].end());
			}
			else {
				result.push_back({ Token::GROUP_LITERAL, Token::TYPE_DEC_LITERAL, index >= argsCount ? 0 : argsCount - index, "", 0 });
			}
		}
		else if (expression[i].Type != Token::TYPE_COMMENT) result.push_back(expression[i]);
	}

	return true;
}