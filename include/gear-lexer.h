/**
 * @file gear-lexer.h
 * @author github.com/TyfoC
 * @brief 
 * @version 1.0
 * @date 4 June 2023
 */

#pragma once
#ifndef GEAR_LEXER
#define GEAR_LEXER

#include <string.h>
#include "gear-grammar.h"
#include "gear-string.h"
#include "gear-array.h"

typedef struct glexeme_t {
	size_t	group;
	size_t	type;
	size_t	position;
	size_t	length;
} glexeme_t;

gbool_t gget_lexeme(const char* source, size_t position, glexeme_t* result);
size_t glex(const char* source, glexeme_t** result);

#endif