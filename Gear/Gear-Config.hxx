#pragma once
#ifndef GEAR_CONFIG
#define GEAR_CONFIG

#include <fstream>
#include "Gear-Lexer.hxx"

namespace Gear {
	class Config {
	public:
		static bool JsonWrite(const std::string path, const std::string& source, const std::vector<Lexeme>& lexemes);
	};
}

#endif