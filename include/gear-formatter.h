#pragma once
#ifndef GEAR_FORMATTER
#define GEAR_FORMATTER

#include "gear-lexer.h"

char* gformat_value(char value);
char* gformat_lexemes(const char* source, const glexeme_t* lexemes, size_t count);

#endif