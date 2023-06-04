#include <locale.h>
#include <gear-status.h>
#include <gear-file.h>
#include <gear-lexer.h>
#include <gear-preprocessor.h>
#include <gear-formatter.h>

enum {
	ACTION_NOT_SELECTED,
	ACTION_INFO,
	ACTION_HELP,
	ACTION_LEX,
	ACTION_PREPROCESS,
};